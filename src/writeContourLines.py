import csv
import arcpy

#Write contour lines to files
#Support Python 2.7
def writeContourLines(input_feature_class):
    i = 0

    with arcpy.da.SearchCursor(input_feature_class, ['Shape@', 'Contour']) as s_cur:
        for row in s_cur:
            polyline = row[0]
            Z = row[1]+0.0

            #Process all polylines
            for feature in polyline:

                #Process polyline
                L=[]
                for point in feature:
                    L.append([point.X, point.Y, Z])

                #Write polyline to CSV file
                z = L[0][2]
                print (z)
                with open(input_feature_class + '_CL_' + str(z)+ "_" + str(i) + ".csv", "wb") as f:
                    wr = csv.writer(f, delimiter=' ')
                    wr.writerows(L)

                i = i + 1

#Export each polyline into separate txt file
input_feature_class =  r'e:\Test\SimplifyContourLinesAXS\data\shp\contour_lines_source_clip.shp'
writeContourLines(input_feature_class)