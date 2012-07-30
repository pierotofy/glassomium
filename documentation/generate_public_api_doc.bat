cd jsdoc-toolkit
java -jar jsrun.jar app\run.js -a -t=templates\jsdoc -d=..\out ..\..\src\server\JS\injectOnLoad.js
cd ..
copy out\symbols\GLA.html .
rd /s /q out
pause