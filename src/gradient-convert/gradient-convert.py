# /usr/bin/python
#
# experimental python wrapper script for cptutils
# $Id$

import os, sys, getopt

# names for types

gnames = {
    'avl' : "ArcView legend",
    'gpl' : "GIMP palette",
    'cpt' : "GMT colour table palette",
    'svg' : "SVG gradient",
    'grd' : "Paint Shop Pro gradient",
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

# cptutils conversion (di)graph

graph = {'avl': ['cpt'],
         'gpl': ['cpt'],
         'cpt': ['svg'],
         'svg': ['cpt','ggr','grd','c3g','gpf','inc','sao','png'],
         'ggr': ['svg','lut'],
         'grd': ['svg'] }

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

    print shortest_path(graph,itype,otype)


# command-line interface

def usage() :
    print "usage : gradient-convert [options] <input> <output>"
    print "options"
    print " -h          : brief help"
    print " -v          : verbose"

def main() :
    try:
        opts,args = getopt.getopt(sys.argv[1:],"hv")
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)

    # defaults
    verbose = False

    for o, a in opts:
        if o == "-h" :
            usage()
            sys.exit(0)
        elif o == "-v" :
            verbose = True
        else:
            assert False, "unhandled option"
            
    if len(args) != 2 :
        usage()
        sys.exit(1)

    ipath, opath = args

    if verbose :
        print "This is gradient-convert"

    opt = (verbose)

    convert(ipath,opath,opt)

    if verbose :
        print "done."

# run main
if __name__ == "__main__":
    main()

