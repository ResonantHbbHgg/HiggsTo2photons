#!/bin/env python

##CASTOR bash script
#ESCAPED=`echo $DIR | sed -e 's/[\\/&]/\\\\&/g'`
#FILES=`nsls ${DIR} | sed "s/^/rfio:${ESCAPED}\//" | paste -s`
#hadd -T ${OUT} ${FILES}

from optparse import OptionParser
from pprint import pprint
import os.path as p
from subprocess import check_call as call

parser = OptionParser(usage="usage: %prog [options] EOS_source_directory\nrun with --help to get list of options")
parser.add_option("--putBack", action="store_true", default=False, help="Put back merged file in source directory [default: %default].")

(options, args) = parser.parse_args()

if len(args) != 1:
    parser.print_usage()
    raise RuntimeError("Need exactly one directory to work with.")

options.inDir = args[0]
options.inDirName = p.basename(options.inDir)

if not options.inDirName:
    raise RuntimeError("Empty target directory name (which defines the sample name). Check path.")

# cmd = """cmsLs %(inDir)s | awk '{ print $5 }' | xargs cmsPfn | sed 's/\?.*$//' > %(inDirName)s.files.txt
# hadd -T %(inDirName)s.pileup.root @%(inDirName)s.files.txt &> %(inDirName)s.pileup.root.log""" 

# call(cmd % vars(options), shell=True)

call( """cmsLs %(inDir)s | awk '{ print $5 }' | xargs cmsPfn | sed 's/\?.*$//' > %(inDirName)s.files.txt""" % vars(options), shell=True)
call( """rm %(inDirName)s.pileup.root %(inDirName)s.pileup.root.log""" % vars(options), shell=True)
call( """hadd -T %(inDirName)s.pileup.root @%(inDirName)s.files.txt &> %(inDirName)s.pileup.root.log""" % vars(options), shell=True)

if options.putBack:
    cmd = """xrdcp %(inDirName)s.pileup.root `cmsPfn %(inDir)s | sed 's/\?.*$//'`/%(inDirName)s.pileup.root"""
    print "Copying back root"
    call(cmd % vars(options), shell=True)
    
    cmd = """xrdcp %(inDirName)s.pileup.root.log `cmsPfn %(inDir)s | sed 's/\?.*$//'`/%(inDirName)s.pileup.root.log"""
    print "Copying back log"
    call(cmd % vars(options), shell=True)

