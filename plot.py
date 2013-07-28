import matplotlib.pyplot as pyplot
import pandas
import sys
import time
from pandas import DataFrame

if __name__ == "__main__":



    FIELD_SEPARATOR = ","
    ENTRY_SEPARATOR = "\n"


    try:
        number = sys.argv[1]
        path = "/Volumes/NO NAME/LOG{number}.TXT".format( number=number )
        path = "/Users/ilyakh/Documents/Arduino/logger_v3/test.txt"
    except IndexError:
        sys.exit("missing log-file number as first command argument");


    # fix data

    with open( path ) as target:
        readings = target.read().strip().split( ENTRY_SEPARATOR )

        split_lines = []

        for r in readings:
            split_lines.append(
                 [ f for f in r.split( FIELD_SEPARATOR ) ]
            )

        # calculate the average number of fields in each entry (on each line)
        field_counts = [ len(l) for l in split_lines ]

        # [/] not a very precise way of doing things, use count vs. product
        average_field_count = float( sum(field_counts) ) / len(field_counts)

        # [i] when you know what the average is, you can warn user if the data is corrupt or not

        if round( average_field_count, 0) == average_field_count:
            print "The number of fields is correct for each reading"
        else:
            print "The number of fields is NOT CORRECT"

        # [->] begins to analyze the type features and ranges

        # remove all lines that do not conform the schema

        print "Average number of fields on each line: ", average_field_count

        lines_with_right_size = []
        for l in split_lines:
            if len(l) == int(round( average_field_count, 0)):
                lines_with_right_size.append(l)

        print "{0} lines were malformed. These are excluded from resulting dataset".format(
            len(split_lines) - len(lines_with_right_size)
        )

        lines_with_right_format = []
        for l in lines_with_right_size:
            try:
                lines_with_right_format.append(
                    [ int(i) for i in l.strip().split( FIELD_SEPARATOR ) ]
                )
            except ValueError:
                continue

        print "{0} lines were malformed. These are excluded from resulting dataset".format(
            len(lines_with_right_size) - len( lines_with_right_format )
        )

        # [->] finally, check ranges








    try:

        data = pandas.io.parsers.read_csv( 
            path,
            index_col=False,
            names=['time',
                   'accel_x', 'accel_y', 'accel_z', 
                   'gyro_x', 'gyro_y', 'gyro_z' ],
            na_values=['nan'],
        )


            

    except IOError:
        sys.exit("file was not found")
    
    

    print "Number of readings:", len( data ) 
    
    p = data.plot()

    timestamps = list( data['time'].values )

    delta_t = []
    for i in range( len(timestamps) -1 ):
        delta_t.append(
            timestamps[i+1] - timestamps[i]
        )

    timedelta_average = ( float(sum( delta_t )) / len( delta_t ) )
    
    timedeltas_above_double_average = \
        [ i for i in delta_t if (i > ( 2.0 * timedelta_average )) ]

    print "Maximal timedelta", max( delta_t )
    print "Average timedelta", timedelta_average
    
    
    timedeltas_above_double_average_percent = \
        float( len( timedeltas_above_double_average ) ) / len( delta_t ) * 100

    print "Timedeltas above double average",
    print len(timedeltas_above_double_average), 
    print round( timedeltas_above_double_average_percent ),
    print "%"

    first_timestamp = timestamps[0]
    last_timestamp = timestamps[-1]

    print "Last timestamp", timestamps[-1]
    print "Maximal timestamp", max( timestamps )
    print "Average frequency", float( len( timestamps ) ) / ( float( last_timestamp - first_timestamp ) / 1000 ) 


    delta_t = DataFrame( delta_t )
    delta_t.plot()
    pyplot.show()

    print "Timedelta standard deviation", float( delta_t.std() )

    font = {
        'family': 'Consolas',
        'weight': 'x-small',
        'size': 11.0,
        'stretch': 0
    }

    
    
    pyplot.rc( 'font', **font )
    pyplot.show( block=True )    
