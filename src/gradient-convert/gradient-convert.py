#! /usr/bin/python
#
# python wrapper script for cptutils
# Copyright (c) J.J. Green 2012, 2014

import os, sys, getopt, tempfile, subprocess, atexit

# version string, set by sed

version = "VERSION"

# list of files to be deleted at exit

delfiles = []
deldirs  = []

# data on gradient types, a hash with values of array of
# [name, aliases, multi-gradient]

gdata = {
    'avl' : ["ArcView legend", []],
    'c3g' : ["CSS3 gradient", ['css3']],
    'cpt' : ["GMT colour table palette", []],
    'ggr' : ["GIMP gradient", []],
    'gpf' : ["Gnuplot palette", []],
    'gpl' : ["GIMP palette", []],
    'grd' : ["Photoshop gradient", ['grd5']],
    'inc' : ["POV-Ray header", ['pov']],
    'lut' : ["Medcon lookup table", []],
    'png' : ["PNG image", []],
    'psp' : ["PaintShop Pro gradient", ['grd3', 'jgd', 'PspGradient']],
    'sao' : ["DS9/SAO colour table", []],
    'svg' : ["SVG gradient", []],
    }

# it is convenient to have separate dicts for each of
# names, typealiases, multi for each type

gnames     = {}
gtypealias = {}

for t, gdatum in gdata.iteritems() :
    gnames[t], gtypealias[t] = gdatum

# generate type dict from alias list

gtypedict = {}
for gtype, galiases in gtypealias.iteritems() :
    gtypedict[gtype] = gtype
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

def capabilities(M, N, A) :

    rfmt, wfmt = rwformats(M, N)

    def quoted(s) :
        return '"'+ s + '"'

    lines = []
    for name in sorted(N.keys()) :
        alias = ", ".join( map(quoted, A[name]) )
        lines.append("%s: {read: %s, write: %s, desc: %s, alias: [%s]}" % \
                         (name,
                          "true" if rfmt[name] else "false",
                          "true" if wfmt[name] else "false",
                          quoted(N[name]),
                          alias))
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

# main function

def convert(ipath, opath, opt) :

    global delfiles
    global deldirs

    verbose, subopts, ifmt, ofmt, burst = opt

    if ifmt is None :
        ifmt = gradtype(ipath)

    if ofmt is None :
        ofmt = gradtype(opath)
    
    if verbose :
        print "input: %s" % (gnames[ifmt])
        print "  %s" % (ipath)
        print "output: %s" % (gnames[ofmt])
        print "  %s" % (opath)

    # for the intermediate filenames; use the basename
    # of the final output file, but make the file location
    # in a tmpname() directory (so that we won't stomp on
    # users local data)

    basename = os.path.splitext( os.path.split(opath)[1] )[0]
    tempdir  = tempfile.mkdtemp()

    deldirs.append(tempdir)

    # this is a bit of a hack to handle grd files

    if burst and (ifmt == 'grd') :
        if verbose :
            print "grd burst call sequence:"
        svgmulti = ("%s/%s-multiple.svg" % (tempdir, basename))  
        svgdir   = ("%s/%s-single" % (tempdir, basename))
        os.mkdir(svgdir)
        deldirs.append(svgdir)
        clists = [['pssvg', '-t', basename + '-%03i', '-o', svgmulti, ipath],
                  ['svgsvg', '-o', svgdir, '-a', svgmulti]]
        for clist in clists :
            if verbose :
                print "  %s" % (" ".join(clist))
            if subprocess.call(clist) != 0 :
                print "failed call to %s : aborting" % (clist[0])
                return False
        delfiles.append(svgmulti)

        for svg in os.listdir(svgdir) :
            svgbase = os.path.splitext(svg)[0]
            ipath2 = "%s/%s" % (svgdir, svg)
            opath2 = "%s/%s.%s" % (opath, svgbase, ofmt)
            opts2  = (verbose, subopts, 'svg', ofmt, False)             
            delfiles.append(ipath2)
            if not convert(ipath2, opath2, opts2) :
                return False

        return True

    # create the system-call sequence, first we create
    # a list of dictionaries of call data

    def pairs(L):
        i = iter(L)
        prev = item = i.next()
        for item in i:
            yield prev, item
            prev = item

    cdlist = []

    callpath = shortest_path(gdgraph, ifmt, ofmt)

    if callpath is None :
        print "cannot convert %s to %s yet, sorry" % (ifmt, ofmt)
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

        path = ("%s/%s.%s" % (tempdir, basename, totype))

        cd0['topath']   = path
        cd1['frompath'] = path

        delfiles.append(path)

    # now run through the call data and make the calls

    if verbose :
        print "call sequence:"

    for cd in cdlist :

        program  = cd['program']
        topath   = cd['topath']
        frompath = cd['frompath']

        clist = [ program, '-o', topath ]
        clist.extend(subopts[program])
        clist.append(frompath)

        if verbose :
            print "  %s" % (" ".join(clist))

        if subprocess.call(clist) != 0 :
            print "failed call to %s : aborting" % (program)
            return False

        if not os.path.exists(topath) :
            print "failed to create %s : aborting" % (topath)
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
        os.unlink(path)

def main() :

    global delfiles
    global deldirs

    try:
        opts, args = getopt.getopt(sys.argv[1:],
                                   "Bb:cf:g:hi:m:n:o:pT:vV",
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
                                    "version"])
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)

    # defaults
    verbose = False
    ifmt    = None
    ofmt    = None
    burst   = False
    subopts = dict((p, []) for p in programs)

    for o, a in opts :
        if o in ("-h", "--help") :
            usage()
            formats_supported(gajmat, gnames)
            sys.exit(0)
        elif o in ("-V", "--version") :
            print "gradient-convert %s" % (version)
            sys.exit(0)
        elif o in ("-c", "--capabilities") :
            capabilities(gajmat, gnames, gtypealias)
            sys.exit(0)
        elif o in ("-B", "--burst") : 
            burst = True
        elif o in ("-g", "--geometry") :
            # geometry only used by svgpng
            subopts['svgpng'].extend([o, a])
            subopts['cptsvg'].extend([o, a])
            subopts['gimpsvg'].extend([o, a])
            subopts['pspsvg'].extend([o, a])
        elif o in ("-b", "--background",
                   "-f", "--foreground",
                   "-n", "--nan") :
            # only affects output to cpt
            subopts['svgcpt'].extend([o, a])
            subopts['gplcpt'].extend([o, a])
            subopts['avlcpt'].extend([o, a])
        elif o in ("-p", "--preview") :
            subopts['cptsvg'].extend([o])
            subopts['gimpsvg'].extend([o])
            subopts['pspsvg'].extend([o])
        elif o in ("-T", "--transparency") :
            # only svg to cpt, gpt, sao
            subopts['svgcpt'].extend([o, a])
            subopts['svggpt'].extend([o, a])
            subopts['svgsao'].extend([o, a])
        elif o in ("-o", "--output-format") :
            ofmt = gtypedict.get(a)
            assert ofmt, "no such output format: %s" % (a)
        elif o in ("-i", "--input-format") :
            ifmt = gtypedict.get(a)
            assert ifmt, "no such input format: %s" % (a)
        elif o in ("-v", "--verbose") :
            verbose = True
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

    opt = (verbose, subopts, ifmt, ofmt, burst)

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

