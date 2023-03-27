
set vardate=%date:~0,4%.%date:~5,2%.%date:~8,2%
set tempdir="Z:\Èí¼ş²¿ÃÅ\AlphaRobot1s_V2.0 pc\"
set destpath=%tempdir%%vardate%
if not exist %destpath% mkdir %destpath%
copy alpharobot1s*.exe %destpath%