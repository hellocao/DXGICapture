@echo off
setlocal enabledelayedexpansion
echo ¿ªÊ¼ÇåÀí...

if exist *.sdf (
	del *.sdf
	echo "É¾³ýÎÄ¼þ" *.sdf
)

if exist *.v12.suo (
	del *.v12.suo /AH
	echo "É¾³ýÎÄ¼þ" *.v12.suo
)

if exist Bin (
	rd Bin /s/q
	echo "É¾³ýÄ¿Â¼" Bin
)

if exist ipch (
	rd ipch /s/q
	echo "É¾³ýÄ¿Â¼" ipch
)

for /r . %%a in (ipch) do (    
  if exist %%a (  
  echo "É¾³ý" %%a  
  rd /s /q "%%a"   
 )  
)

for /r . %%a in (*.sdf) do (    
  if exist %%a (  
  echo "É¾³ý" %%a  
  del "%%a"   
 )  
)

for /r . %%a in (Release) do (    
  if exist %%a (  
  echo "É¾³ý" %%a  
  rd /s /q "%%a"   
 )  
)

for /r . %%a in (Debug) do (    
  if exist %%a (  
  echo "É¾³ý" %%a  
  rd /s /q "%%a"   
 )  
)

for /r . %%a in (obj) do (    
  if exist %%a (  
  echo "É¾³ý" %%a  
  rd /s /q "%%a"   
 )  
)

pause
