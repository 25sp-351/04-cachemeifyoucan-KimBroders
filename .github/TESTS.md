Test for running without -q or filename.
    command-line argument: ./main.exe 
    expected output:
        Usage: C:\Users\kim\source\CPSC-351\01-rods-KimBroders\.github\main.exe [-q] <segment lengths filename>

Test for running with -q but without filename.
    command-line argument: ./main.exe -q
    expected output:
        Usage: C:\Users\kim\source\CPSC-351\01-rods-KimBroders\.github\main.exe [-q] <segment lengths filename>

Test for reading an invalid file. 
    command-line argument: ./main.exe bogusfile
    expected output: Error open file bogusfile

Test for invalid rod lengths.
    command-line argument: ./main.exe data.txt
    rod length inputs:
        f
        -1
    expected output:
        Enter rod length:
        Invalid input: f

        Enter rod length: -1
        Invalid input: -1

Test for rod length shorter than longest segment.
    rod length: 10
    segment lengths:
        5, 18
        10, 81
        16, 178
    expected output:
        {
        "input_length": 10,
        "value": 81,
        "remainder": 0,
        "cuts": [
        { "length": 5, "count": 0,
        "piece_value": 18, "value": 0},
        { "length": 10, "count": 1,
        "piece_value": 81, "value": 81},
        { "length": 16, "count": 0,
        "piece_value": 178, "value": 0}
        ] }
        
Test for rod length shorter than shortest segment.
    rod length: 6
    segment lengths:
        10, 73
    expected output:
        0 @ 10 = 0
        Remainder: 6
        Value: 0
    expected output:
        {
        "input_length": 6,
        "value": 0,
        "remainder": 6,
        "cuts": [
        { "length": 10, "count": 0,
        "piece_value": 73, "value": 0}
        ] }

Test for use multiple smaller segments instead of one longer segment
    rod length: 7
    segment lengths:
        3, 10
        4, 14
        5, 17
    expected output:
        {
        "input_length": 7,
        "value": 24,
        "remainder": 0,
        "cuts": [
        { "length": 3, "count": 1,
        "piece_value": 10, "value": 10},
        { "length": 4, "count": 1,
        "piece_value": 14, "value": 14},
        { "length": 5, "count": 0,
        "piece_value": 17, "value": 0}
        ] }

Test for general input
    rod length: 47
    segment lengths:
        10, 100
        5, 27
        2, 3
    expected output:
        {
        "input_length": 47,
        "value": 430,
        "remainder": 0,
        "cuts": [
        { "length": 10, "count": 4,
        "piece_value": 100, "value": 400},
        { "length": 5, "count": 1,
        "piece_value": 27, "value": 27},
        { "length": 2, "count": 1,
        "piece_value": 3, "value": 3}
        ] }

Removed excessive tests.