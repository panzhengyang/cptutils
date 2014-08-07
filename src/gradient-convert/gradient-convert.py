#! /usr/bin/python
#
# python wrapper script for cptutils
# Copyright (c) J.J. Green 2012, 2014

import os, sys, getopt, tempfile, subprocess, atexit

# version string, set by sed

version = "VERSION"

# list of files & directories to be deleted at exit

delfiles = []
deldirs  = []

# data on gradient types, a hash with values of array of
# [name, aliases, ext, burstable]

gdata = {
    'avl' : ['ArcView legend', [], 'avl', False],
    'c3g' : ['CSS3 gradient', ['css3'], 'c3g', False],
    'cpt' : ['GMT colour table palette', [], 'cpt', False],
    'ggr' : ['GIMP gradient', [], 'ggr', False],
    'gpf' : ['Gnuplot palette', [], 'gpf', False],
    'gpl' : ['GIMP palette', [], 'gpl', False],
    'grd' : ['Photoshop gradient', ['grd5'], 'grd', True],
    'inc' : ['POV-Ray header', ['pov'], 'inc', False],
    'lut' : ['Medcon lookup table', [], 'lut', False],
    'png' : ['PNG image', [], 'png', False],
    'psp' : ['PaintShop Pro gradient', ['grd3', 'jgd'], 'PspGradient', False],
    'sao' : ['DS9/SAO colour table', [], 'sao', False],
    'svg' : ['SVG gradient', [], 'svg', True],
    }

# it is convenient to have separate dicts for each of
# names, typealiases, multi for each type

gnames     = {}
gtypealias = {}
gburstable = {}
gexts      = {}

for t, gdatum in gdata.iteritems() :
    gnames[t], gtypealias[t], gexts[t], gburstable[t] = gdatum

# generate type dict from alias list, this used to specify
# types (eg -i psp) and to guess file-types from extensions.

gtypedict = {}
for gtype, galiases in gtypealias.iteritems() :
    gtypedict[gtype] = gtype
    gtypedict[gexts[gtype]] = gtype
    for galias in galiases :
        gtypedict[galias] = gtype

# conversion adjacency matrix, implemented as a dict-of-dicts
# with the entries being the conversion programs

gajmat = {
    'avl' : { 'cpt' : 'avlcpt' },
    'gpl' : { 'cpt' : 'gplcpt' },
    'cpt' : { 'svg' : 'cptsvg' },
    'svg' : { 'cpt' : 'svgcpt',
              'ggr' : 'svggimp',
              'psp' : 'svgpsp',
              'c3g' : 'svgcss3',
              'gpf' : 'svggpt',
              'inc' : 'svgpov',
              'sao' : 'svgsao',
              'png' : 'svgpng' },
    'ggr' : { 'svg' : 'gimpsvg',
              'lut' : 'gimplut' },
    'psp' : { 'svg' : 'pspsvg' },
    'grd' : { 'svg' : 'pssvg' }
    }

# create the conversion (di)graph from the adjacency
# matrix

gdgraph = {}

for t0, t1d in gajmat.iteritems() :
    gdgraph[t0] = t1d.keys()

# also create a list of programs 

programs = []

for t in gajmat.values() :
    for s in t.values() :
        programs.append(s)

programs.append('svgsvg')

# utlity function for the subsequent print functions

def rwformats(M, N) :

    rfmt = {}
    wfmt = {}

    for name in N.keys() :
        rfmt[name] = False
        wfmt[name] = False

    for nr in M.keys() :
        rfmt[nr] = True
        for nw in M[nr].keys() :
            wfmt[nw] = True

    return (rfmt, wfmt)

# print the formats supported from the adjacency
# matrix M and name -> description dict N

def formats_supported(M, N) :

    rfmt, wfmt = rwformats(M, N)

    print "supported formats:"

    for name in sorted(N.keys()) :
        print "| %3s | %-25s | %s%s |" % \
            (name, N[name],
             "R" if rfmt[name] else "-",
             "W" if wfmt[name] else "-")

# likewise, but in YAML format (it would not be difficult
# to make this JSON)

def capabilities() :

    rfmt, wfmt = rwformats(gajmat, gnames)

    def quoted(s) :
        return '"'+ s + '"'

    def boolstring(val) :
        return "true" if val else "false"

    lines = []
    fmt = "%s: {read: %5s, write: %5s, burst: %5s, desc: %s, alias: [%s], ext: %s}"
    for name in sorted(gnames.keys()) :
        alias = ", ".join( map(quoted, gtypealias[name]) )
        lines.append(fmt % \
                         (name,
                          boolstring(rfmt[name]),
                          boolstring(wfmt[name]),
                          boolstring(gburstable[name]),
                          quoted(gnames[name]),
                          alias,
                          quoted(gexts[name])
                          ))
    print "# gradient-convert %s capabilities" % (version)
    print "{\n  %s\n}" % (",\n  ".join(lines))

# taken from http://www.python.org/doc/essays/graphs.html
# this simple shortest path code determines the call sequence

def shortest_path(graph, start, end, path=[]):
    path = path + [start]
    if start == end:
        return path
    if not graph.has_key(start):
        return None
    shortest = None
    for node in graph[start]:
        if node not in path:
            newpath = shortest_path(graph, node, end, path)
            if newpath:
                if not shortest or len(newpath) < len(shortest):
                    shortest = newpath
    return shortest

# determine type from path

def gradtype(path) :

    ext = os.path.splitext(path)[1][1:]
    if not ext:
        print("cannot determine file extension for %s" % path)
        sys.exit(1)

    gtype = gtypedict.get(ext, None)
    if gtype is None:
        print("unknown gradient extension %s" % ext)
        formats_supported(gajmat, gnames)
        sys.exit(1)

    return gtype


def run_clist(clist, topath, verbose) :
    if verbose :
        print "  %s" % (" ".join(clist))
    if subprocess.call(clist) != 0 :
        print "failed call to %s : aborting" % (clist[0])
        return False
    if topath and not os.path.exists(topath) :
        print "failed to create %s : aborting" % (topath)
        return False
    return True

# main function

def convert(ipath, opath, opt) :

    global delfiles
    global deldirs

    # for the intermediate filenames; use the basename
    # of the input file, but make the file location
    # in a tmpname() directory (so that we won't stomp on
    # users local data)

    tempdir = tempfile.mkdtemp()
    deldirs.append(tempdir)

    # Here we handle the multiple-gradient files, although this
    # is a bit convoluted it is much less messy than my attempt
    # to do this in a generic fashion; we'd not expect to support
    # many other multi-gradient formats in any case

    if opt['burst'] : 

        # basename used in the title, so make it meaningful,
        # we don't take it from the output (as that will ba a 
        # directory, and may well be ".")

        basename = os.path.splitext( os.path.split( opt['ipath'] )[1] )[0]

        if opt['ifmt'] == 'grd' :

            # input is a single grd file, convert it to a single 
            # svg file with muliple gradients; then call convert()
            # with that file as input and burst = True (so that
            # we execute the ifmt == 'svg' case below).
            # counting the gradient to reduce the redundant zeros

            svgmulti = "%s/%s.svg" % (tempdir, basename)  
            clist = ['pssvg', '-o', svgmulti, opt['ipath']]
            if opt['verbose'] :
                print "  %s" % (" ".join(clist))
            if subprocess.call(clist) != 0 :
                print "failed call to %s : aborting" % (clist[0])
                return False
            delfiles.append(svgmulti)

            opt['ifmt']  = 'svg'
            opt['burst'] = True

            return convert(svgmulti, opath, opt)

        elif opt['ifmt'] == 'svg' :

            # input is a single svg file (which may be from the 
            # case above, or an original infile).

            if opt['ofmt'] == 'svg' :
                # final output is svg, burst to output directory
                clist = ['svgsvg', '-o', opath, '-a', ipath]
                clist.extend(opt['subopts']['svgsvg'])
                if not run_clist(clist, opath, opt['verbose']) :
                    return False
            else :
                # final output is not svg, so burst into a temp
                # directory, then call convert() on each file in
                # that directory, this time with burst = False
                svgdir = "%s/%s" % (tempdir, basename)
                os.mkdir(svgdir)
                deldirs.append(svgdir)
                clist = ['svgsvg', '-o', svgdir, '-a', ipath]
                if not run_clist(clist, None, opt['verbose']) :
                    return False
                opt['ifmt']  = 'svg'
                opt['burst'] = False
                for svg in os.listdir(svgdir) :
                    svgbase = os.path.splitext(svg)[0]
                    ipath2 = "%s/%s" % (svgdir, svg)
                    opath2 = "%s/%s.%s" % (opath, svgbase, gexts[opt['ofmt']])
                    delfiles.append(ipath2)
                    if not convert(ipath2, opath2, opt) :
                        return False
            return True

    # basename for non-burst is only used in temporary files

    basename = "cptutils"

    # create the system-call sequence, first we create
    # a list of dictionaries of call data

    def pairs(L):
        i = iter(L)
        prev = item = i.next()
        for item in i:
            yield prev, item
            prev = item

    cdlist = []

    callpath = shortest_path(gdgraph, opt['ifmt'], opt['ofmt'])

    if callpath is None :
        print "cannot convert %s to %s yet, sorry" % (opt['ifmt'], opt['ofmt'])
        formats_supported(gajmat, gnames)
        return None

    for t0, t1 in pairs( callpath ) :
        cd = {
            'fromtype' : t0,
            'totype'   : t1,
            'program'  : gajmat[t0][t1]
            }
        cdlist.append(cd)

    # add input/output filenames

    cdlist[0]['frompath'] = ipath
    cdlist[-1]['topath']  = opath

    # add temporary filenames (also added to the global
    # delfiles list used by the cleanup function)

    for cd0, cd1 in pairs(cdlist) :

        totype = cd0['totype']
        path = ("%s/%s.%s" % (tempdir, basename, gexts[totype]))

        cd0['topath']   = path
        cd1['frompath'] = path

        delfiles.append(path)

    # now run through the call data and make the calls

    for cd in cdlist :

        program  = cd['program']
        topath   = cd['topath']
        frompath = cd['frompath']

        clist = [program, '-o', topath]
        clist.extend(opt['subopts'][program])
        clist.append(frompath)

        if not run_clist(clist, topath, opt['verbose']) :
            return False

    return True

# command-line interface

def usage() :
    print "usage : gradient-convert [options] <input> <output>"
    print "options"
    print " -b rgb      : background (cpt)"
    print " -B          : burst multiple gradients"
    print " -c          : print program capabilites in YAML format"
    print " -f rgb      : foreground (cpt)"
    print " -g geometry : geometry (png, svg)"
    print " -h          : brief help"
    print " -i format   : format of input file"
    print " -n rgb      : nan colour (cpt)"
    print " -o format   : format of output file"
    print " -p          : preview (svg)"
    print " -T rgb      : transparency (cpt, gpt, sao)"
    print " -v          : verbose"
    print " -V          : version"
    print " -z          : zip multiple gradients"
    print "the type in brackets indicates the file type affected"
    print

def cleanup_dirs(verbose) :
    global deldirs
    for path in reversed(deldirs) :
        if verbose :
            print path
        os.rmdir(path)

def cleanup_files(verbose) :
    global delfiles
    for path in delfiles :
        if verbose :
            print path
        if os.path.isfile(path) :
            os.unlink(path)

def main() :

    global delfiles
    global deldirs

    try:
        opts, args = getopt.getopt(sys.argv[1:],
                                   "Bb:cf:g:hi:m:n:o:pT:vVz",
                                   ["burst",
                                    "background=",
                                    "capabilities",
                                    "foreground=",
                                    "geometry=",
                                    "help",
                                    "input-format=",
                                    "mode=",
                                    "nan=",
                                    "output-format=",
                                    "preview"
                                    "transparency=",
                                    "verbose",
                                    "version",
                                    "zip"])
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)

    # defaults
    verbose = False
    ifmt    = None
    ofmt    = None
    burst   = False
    zipped  = False
    subopts = dict((p, []) for p in programs)

    # progs_<x> accept the -<x> option
    progs_p   = ['cptsvg', 'gimpsvg', 'pspsvg', 'svgsvg']
    progs_g   = ['svgpng'] + progs_p
    progs_bfn = ['svgcpt', 'gplcpt', 'avlcpt']
    progs_T   = ['svgcpt', 'svggpt', 'svgsao']

    for o, a in opts :
        if o in ("-h", "--help") :
            usage()
            formats_supported(gajmat, gnames)
            sys.exit(0)
        elif o in ("-V", "--version") :
            print "gradient-convert %s" % (version)
            sys.exit(0)
        elif o in ("-c", "--capabilities") :
            capabilities()
            sys.exit(0)
        elif o in ("-B", "--burst") : 
            burst = True
        elif o in ("-g", "--geometry") :
            for prog in progs_g : 
                subopts[prog].extend([o, a])
        elif o in ("-b", "--background",
                   "-f", "--foreground",
                   "-n", "--nan") :
            for prog in progs_bfn : 
                subopts[prog].extend([o, a])
        elif o in ("-p", "--preview") :
            for prog in progs_p : 
                subopts[prog].extend([o])
        elif o in ("-T", "--transparency") :
            for prog in progs_T : 
                subopts[prog].extend([o, a])
        elif o in ("-o", "--output-format") :
            ofmt = gtypedict.get(a)
            assert ofmt, "no such output format: %s" % (a)
        elif o in ("-i", "--input-format") :
            ifmt = gtypedict.get(a)
            assert ifmt, "no such input format: %s" % (a)
        elif o in ("-v", "--verbose") :
            verbose = True
        elif o in ("-z", "--zip") :
            zipped = True
        else:
            assert False, "unhandled option"

    if len(args) != 2 :
        usage()
        sys.exit(1)

    ipath, opath = args

    if verbose :
        print "This is gradient-convert (version %s)" % (version)

    atexit.register(cleanup_dirs, False)
    atexit.register(cleanup_files, False)

    if ifmt is None :
        ifmt = gradtype(ipath)

    if ofmt is None :
        if burst or zipped :
            print "Output format must be specified (see -o option)"
            sys.exit(1)
        ofmt = gradtype(opath)
    
    if verbose :
        print "input: %s" % (gnames[ifmt])
        print "  %s" % (ipath)
        print "output: %s" % (gnames[ofmt])
        print "  %s" % (opath)
        print "call sequence:"

    opt = { 'verbose' : verbose, 
            'subopts' : subopts, 
            'ifmt'    : ifmt, 
            'ofmt'    : ofmt, 
            'burst'   : burst, 
            'zipped'  : zipped }

    success = convert(ipath, opath, opt)

    if verbose :
        print "done."

    if success is True :
        sys.exit(0)
    else :
        sys.exit(1)

# run main
if __name__ == "__main__":
    main()

