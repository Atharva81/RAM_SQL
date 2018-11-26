# RAM_SQL
A ground up, RAM based implementation of SQL using C++


Instructions
-------------------------------
>> To compile and generate executable file, type 'make' (without quotes)

>> An executable file named 'dbms' will be generated

>> Type ./dbms to run it

>> Type your SQL queries after the ">>" prompt and suffix it with ';'

>> To use an input file, use "./dbms < input_file_name".
    Make sure to give "exit;" command in the end to avoid infinite loop
    
>> A sample input file named input.txt has been provided. Type ./dbms < input.txt to run it.

>> To delete the generated executable file and log file, type 'make clean'


Things you can do with this RAM Based DBMS:
--------------------------------------------
>> Create table (with checks for default, not null etc)

>> Insert command (either insert in all columns or selected columns (not inserting in column without default will raise ERROR))

>> select command, with conditions after where clause

>> In, not in

>> Nested queries (nested two levels)

>> drop table

>> Aliases: can provide alias of any length to any column (not just single letter)

>> delete tuples from a table in database

>> Queries can be given in any case. They're case insensitive, like SQL queries.

>> Multiple spaces or newlines can be given between tokens in a query. The parser will handle it.

..........And much more


Control Flow of a few example queries:
--------------------------------------
>>> create table students(Name varchar(20) not null,Roll int not null,Age int default 20);

> Remove white spaces and insert it into tokens vector

> create function is invoked because tokens[0]="create"

> Write info to log file, with timestamp

> Table with same name doesn't exist, so enter table name into tables vector

> Set Name as varchar(20) not null

> Set Roll as int not null

> Set Age as int default = 20

>>> drop table students

> Remove white spaces and insert it into tokens vector

> drop function is invoked because tokens[0]="drop"

> Write info to log file, with timestamp

> Remove entry from table, db, and col vectors

>>> INSERT into students(Name,Roll) values("Ayush",5);

> Remove white spaces and insert it into tokens vector

> insert function is invoked because tokens[0]="insert"

> Write info to log file, with timestamp

> Enter "Ayush" in Name

> Enter 5 in Roll

> Age value is not specified, but it has default value 20, so 20 is inserted


>>SELECT * from students WHERE name in ( "Atharva", "Pratik");

> Remove white spaces and insert into token vector

> select function is invoked because tokens[0]="select"

> Write info to log file, with timestamp

> find name of table and check in database

> check for where keyword and invokes it

> In where it goes to else condition

> here it finds "in" keyword and then create vector<string> temp

> since there is no nested select statement, it will go to else part again.

> push in result the corresponding row_ids which satisfy condition and return result

> come back to select function, since it is * it will push all cols and return result

> now print is called with result as argument

> it will print all the corresponding rows and columns

>> DELETE from students where name="Pranav" or Roll>10;

> Remove white spaces and insert into token vector

> delete function is invoked because tokens[0]="delete"

> Write info to log file, with timestamp

> find name of table and check in database

> check for where keyword and invokes it

> In where it goes to else condition

> calls RELOP function 

> then check "or" keyword and implement it

> return array of rowids needs to be deleted

> come back to delete function

> now delete corresponding rows
