# Simplify contour lines using axial splines
Basic properties and features of the simplification algorithm, the input data and provided results, are described.


## 1.1. Source code
The source code of the simplification software written in C++ (C++ 2020) is bundled as the Visual Studio 2022 project. 
It depends on the Eigen library
- Eigen, https://gitlab.com/libeigen/eigen, 
which is bundled.

## 1.2. Running the software

The binary version (64 bit, VS 2022 compiler) of the clusterization software

     simplifyAXS.exe

can be found in \bin directory. To run the software, open the command prompt and use the following combination of parameters, values, and options: 

	simplifyAXS.exe file_name +parameter1=value1 +parameter2=value2 -option

#### Note:
The generated output dxf file in the  destination folder, containing results, will be overwritten. For further details, see Sec. 1.6.

## 1.3 Input files

The simplification algorithm requires fhe following data:
- Contour line c(h) of the given height h.
- Vertical buffer formed by the lower contour c(h - dh).
- Vertical buffer formed by the upper contour c(h + dh).

![ABN](./data/contours_buffers_fig2.jpg)

Each object, represented by contour line fragment or vertical vertical buffer, is stored in a separate CSV file. The input file contains the Cartesian coordinates X, Y, Z of the vertices of contour lines and buffers.

Source contour c(h), h=271 m, file 'contour_271.0_37.csv.
The associated CSV file name contains its height (271.0) and unique id(37)

	716760.3952809327 984135.9742799804 271.0
	716758.6807809327 984138.3343799805 271.0
	716752.8364809327 984146.2996799804 271.0
	716746.4332809327 984157.8285799804 271.0
	716742.3027809327 984165.8620799804 271.0
	716740.5406809327 984172.0404799804 271.0
	716738.4404809327 984176.2855799805 271.0
	716734.6108809327 984180.2318799804 271.0
	716729.0500809327 984184.3721799804 271.0
	716722.7397809327 984188.0845799805 271.0

All files in the destination folder, compliant with the user-defined contour mask

     +cont=*contour_lines*.csv
     
are processed one by one.

Lower part of the vertical buffer c(h - dh), dh = 0.1, h - dh = 270.9, file = 'buffer_B1_270.9_27.csv', example:
The associated CSV file name contains its height (270.9) and unique id(27).

	716659.8204298521 984236.4795014771 270.9
	716660.3242005465 984236.3242001563 270.9
	716660.5701419857 984236.2550386918 270.9
	716660.8453549680 984236.1546448352 270.9
	716660.8160002468 984236.5687168138 270.9
	716660.6305784797 984236.6305785101 270.9
	716660.5073158112 984236.7051947351 270.9
	716660.2971106865 984236.7028892142 270.9
	716659.8204298521 984236.5545886704 270.9
	
All files in the destination folder, compliant with the user-defined buffer mask

     +buff1=*buffer_B1*.csv
     
are processed one by one.
	
Upper part of the vertical buffer c(h + dh), dh = 0.1, h + dh = 271.1, file = 'buffer_B2_271.1_3.csv', example:
The associated CSV file name contains its height (271.1) and unique id(3).

	716779.5757762302 984150.4635644477 271.1
	716779.5370183459 984150.4999999999 271.1
	716779.5164838363 984150.5164834909 271.1
	716779.4999750123 984150.5319453202 271.1
	716779.3329811768 984150.6670188621 271.1
	716778.9616591753 984150.9616592828 271.1
	716778.5000125975 984151.3996718017 271.1
	716778.4438825462 984151.4438826495 271.1
	716778.3845153574 984151.5065083974 271.1
	
All files in the destination folder, compliant with the user-defined buffer mask

     +buff2=*buffer_B2*.csv
     
are processed one by one.

### 1.31 Conversion of files
The bundled Python script

     writeContourLines.py
     
 converts the input shape file to the desired CSV format. All contour lines are processed sequentially. It is necessary to set the full path to the shp file
 
     #Export each polyline into separate txt file
     input_feature_class =  r'e:\Test\SimplifyContourLinesAXS\data\shp\contour_lines_source_clip.shp'
     writeContourLines(input_feature_class)
     
 It depends on the ArcPy library (2.7 or 3.6 versions are supported):
 
           https://www.esri.com/en-us/arcgis/products/arcgis-python-libraries/libraries/arcpy

## 1.4 List of parameters


### 1.4.1 Setting the weighted version

The weighted version of the generalization tool is run using -w switch. The weight wi depends on the angle ωi between the segments. Because the inverted matrices can not be precomputed, the processing is significantly slower.

#### Example:
*Weighted version of the partial dispacement*

     simplifyAXS.exe -w +buff1=*buffer_B1*.csv +buff2=*buffer_B2*.csv +cont=*contour_lines*.csv
     
     
### 1.4.2 Setting the smoothness

User-defined smoothness factor can be set using the parameter "lambda1"

	+lambda1=val
	
$\lambda_{1}$ regulates the smoothing of simplified contour lines. For common contour topographic maps, $\lambda_{1}$ is
set so that $\lambda_{1}\in[1,10] $; the contour lines are perceived as naturally smooth.  For highly oscillating 
contour lines acquired from the point cloud, this value may be significantly higher, and $\lambda_{1}\in 
[1000,10000]$. 

#### Example:
*Non-weighted version of the partial dispacement with the smoothing factor $\lambda_{1}=2$*

     simplifyAXS.exe +lambda1=2 +buff1=*buffer_B1*.csv +buff2=*buffer_B2*.csv +cont=*contour_lines*.csv

![ABN](./data/variable_alpha2.jpg)

### 1.4.3 Setting the symmetry

User-defined symmetry factor can be set using the parameter "lambda2"

	+lambda2=val
	
$\lambda_{2}$ regulates the symmetry of simplified contour line due to the vertical buffer. Increasing the values of λ2, the simplified contour gradually moves to the center of vertical buffer. Its values are slightly smaller than $\lambda_{1}$, we set $\lambda_{2}=$\lambda_{1}/2$.

#### Example:
*Non-weighted version of the partial dispacement with the smoothing factor $\lambda_{1}=2$ and symmetry factor $\lambda_{1}=2$*

     simplifyAXS.exe +lambda1=2 +lambda2=2 +buff1=*buffer_B1*.csv +buff2=*buffer_B2*.csv +cont=*contour_lines*.csv

![ABN](./data/smoothing_lambda_compare.jpg)

The resulting solution is a balanced combination of three factors, shape similarity,
smoothness, and symmetry, regulated by three input parameters
$w_{i}$, $\lambda_{1}$, and $\lambda_{2}$.



