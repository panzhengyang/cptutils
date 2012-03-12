#! /usr/bin/python
#
# experimental python wrapper script for cptutils
# Copyright (c) J.J. Green 2012
#
# $Id: gradient-convert.py,v 1.1 2012/03/12 01:11:32 jjg Exp jjg $

import os, sys, getopt, tempfile, subprocess, atexit

# version string, set by sed

version = "VERSION"

# list of files to be deleted at exit

delfiles = []

# names for types

gnames = {
    'avl' : "ArcView legend",
    'gpl' : "GIMP palette",
    'cpt' : "GMT colour table palette",
    'svg' : "SVG gradient",
    'grd' : "Paint Shop Pro gradient",
    'ggr' : "GIMP gradient",
    'c3g' : "CSS3 gradient",
    'gpf' : "Gnuplot palette",
    'inc' : "POV-Ray header",
    'sao' : "DS9/SAO colour table",
    'lut' : "Medcon lookup table",
    'png' : "PNG image"
    }

# aliases for types

gtypealias = {
    'avl' : [],
    'gpl' : [],
    'cpt' : [],
    'svg' : [],
    'ggr' : [],
    'grd' : ['jgd','PspGradient'],
    'c3g' : ['css3'],
    'gpf' : [],
    'inc' : ['pov'],
    'sao' : [],
    'lut' : [],
    'png' : []
    }

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
              'grd' : 'svgpsp',
              'c3g' : 'svgcss3',
              'gpf' : 'svggpt',
              'inc' : 'svgpov',
              'sao' : 'svgsao',
              'png' : 'svgpng' },
    'ggr' : { 'svg' : 'gimpsvg',
              'lut' : 'gimplut' },
    'grd' : { 'svg' : 'pspsvg' }
    }

# create the conversion (di)graph from the adjacency
# matrix

graph = {}

for t0, t1d in gajmat.iteritems() :
    graph[t0] = t1d.keys()

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
        print("cannot detemine file extension for %s" % path)
        exit(1)

    gtype = gtypedict.get(ext,None)
    if gtype is None:
        print("unknown gradient extension %s" % ext)
        exit(1)

    return gtype

# main function

def convert(ipath,opath,opt) :

    verbose = opt
    itype = gradtype(ipath)
    otype = gradtype(opath)
    
    if verbose :
        print "input: %s" % (gnames[itype])
        print "  %s" % (ipath)
        print "output: %s" % (gnames[otype])
        print "  %s" % (opath)

    # create the system-call sequence, first we create
    # a list of dictionaries of call data

    def pairs(L):
        i = iter(L)
        prev = item = i.next()
        for item in i:
            yield prev, item
            prev = item

    cdlist = []

    callpath = shortest_path(graph,itype,otype)

    if callpath is None :
        print "cannot convert %s to %s yet, sorry" % (itype,otype)
        return None

    for t0,t1 in pairs(shortest_path(graph,itype,otype)) :
        cd = {
            'fromtype' : t0,
            'totype'   : t1,
            'program'  : gajmat[t0][t1]
            }

        cdlist.append(cd)

    # add input/output filenames

    cdlist[0]['frompath'] = ipath
    cdlist[-1]['topath']  = opath

    # add the intermediate filenames; use the basename
    # of the final output file, but make the file location
    # in a tmpname() directory (so that we won't stomp on
    # users local data)

    basename = os.path.splitext(opath)[0]
    tempdir = tempfile.mkdtemp()

    # register the cleanup function to empty the temporary
    # directory of files and remove it

    def cleanup(verbose) :
        if delfiles and verbose :
            print "deleting"
        for delfile in delfiles :
            if os.path.exists(delfile) :
                os.unlink(delfile)
                if verbose :
                    print "  %s" % (delfile)
        os.rmdir(tempdir)
        if verbose :
            print "  %s" % (tempdir)

    atexit.register(cleanup,False)

    # add temporary filenames (also added to the global
    # delfiles list used by the cleanup function)

    for cd0,cd1 in pairs(cdlist) :

        totype = cd0['totype']

        path = ("%s/%s.%s" % (tempdir,basename,totype))

        cd0['topath']   = path
        cd1['frompath'] = path

        delfiles.append(path)

    # now run through the call data and make the calls

    if verbose :
        print "call sequence:"

    for cd in cdlist :

        clist = [
            cd['program'],
            '-o', 
            cd['topath'],
            cd['frompath']
            ]

        if verbose :
            print "  %s" % (" ".join(clist))

        if subprocess.call(clist) != 0 :
            print "failed call to %s : aborting" % (cd['program'])
            return None

        if not os.path.exists(cd['topath']) :
            print "failed to create %s : aborting" % (cd['topath'])
            return None

# command-line interface

def usage() :
    print "usage : gradient-convert [options] <input> <output>"
    print "options"
    print " -h          : brief help"
    print " -v          : verbose"
    print " -V          : version"

def main() :
    try:
        opts,args = getopt.getopt(sys.argv[1:],
                                  "hvV",
                                  ["help","verbose","version"])
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)

    # defaults
    verbose = False

    for o, a in opts:
        if o in ("-h", "--help") :
            usage()
            sys.exit(0)
        elif o in ("-V", "--version") :
            print "gradient-convert %s" % (version)
            sys.exit(0)
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

    opt = (verbose)

    retval = convert(ipath,opath,opt)

    if verbose :
        print "done."

    if retval is None :
        exit(1)
    else :
        exit(0)

# run main
if __name__ == "__main__":
    main()

