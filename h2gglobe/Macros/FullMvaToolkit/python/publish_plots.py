import os,sys,fnmatch

def printLinks(outFile,plotTypes,plotNames,wsName):
  outFile.write('Author: Matthew Kenzie <br>')
  outFile.write('<script language="Javascript"> \n document.write("Last modified: " + document.lastModified + ""); \n </SCRIPT> <br> \n ')
  outFile.write('<center> \n <p> \n <font size="5">FullMvaToolkit Diagnostics</font> \n </p> \n')
  outFile.write('<a href=\"home.html\">Home</a> <br> \n')
  outFile.write('Rewritten workspace to file: <br>'+wsName+'<br>\n')
  outFile.write('<table>\n')
  for p,plot in enumerate(plotTypes):
    if p==0 or p==3 or p==6: outFile.write('\t<tr> \n')
    outFile.write('\t\t <td> <a href=\"'+plot+'.html\">'+plotNames[p]+'</a> &nbsp </td> \n')
    if p==2 or p==5 or p==8: outFile.write('\t</tr> \n')
  outFile.write('</table>')
  outFile.write('<br>\n')
  outFile.write('<br>\n')
  outFile.write('</center>')

wsName=sys.argv[1]

paths=[]

oldMass="blank"

plotTypes=['model','diff_model','fit','dat_sideband','bkg_sideband','interp','uncorrErr','fCorr','fCovar']
plotNames=['Corrected background model','Data-background difference','Background Model Fits','Data in sidebands','Background MC in sidebands','Signal interpolation','Uncorrelated error matrices','Fractional correlation matrices','Fractional covariance matrices']
modelPlots=[]
diffPlots=[]
fitPlots=[]
datSidePlots=[]
bkgSidePlots=[]
interpPlots=[]
uncorrErrPlots=[]
fCorrPlots=[]
fCovarPlots=[]
plots=[modelPlots,diffPlots,fitPlots,datSidePlots,bkgSidePlots,interpPlots,uncorrErrPlots,fCorrPlots,fCovarPlots]

path='plots/png'
homepage = open(path+'/home.html','w')
printLinks(homepage,plotTypes,plotNames,wsName)
homepage.write('<center>\n')
homepage.write('<font size="5"> Homepage </font> <br> \n')
homepage.write('Click links above to browse plots <br> \n')
homepage.write('<img height=\"200\" src=\"PhoPhoDraw.png\"> \n')
homepage.write('<img height=\"250\" src=\"PhotPhotEvent.png\"> \n')
homepage.write('</center>\n')

for root, dirs, files in os.walk(path):
  for p,pType in enumerate(plotTypes):
    for filename in fnmatch.filter(files,pType+"*.png"):
      plots[p].append(filename)
    
    plots[p].sort()
    htmlFile = open(path+'/'+plotTypes[p]+'.html','w')
    printLinks(htmlFile,plotTypes,plotNames,wsName);
    for plot in plots[p]:
      if plotTypes[p]=='fit':
        name = plot.split('.png')[0]
        tArr=name.split('_')
        t=tArr[1]
        mass=t[1:]
        if name=='fit_all_norm': mass='all'
        if mass==oldMass:
          htmlFile.write('<a href='+plot+'><img width=\"300\" src=\"'+plot+'\"></a>\n')
        else:
          htmlFile.write('<br> <b> <font color=\"blue\"> Mass '+mass+':</font></b><br> <br> <a href='+plot+'><img width=\"500\" src=\"'+plot+'\"></a> <br> \n')
        oldMass=mass
      else: 
        htmlFile.write('<a href='+plot+'><img height=\"400\" src=\"'+plot+'\"></a>\n')


