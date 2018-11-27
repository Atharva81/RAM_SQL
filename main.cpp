/**
    CS-3700 Asn 2, main.cpp
    Purpose: To create a RAM based DBMS

    @author Ayush Kumar
    @author Pranav Jha
    @author Atharva Gupta
    @author Pratik Agrawalla
    @version 1.1 26/11/18 
*/

#include <bits/stdc++.h>
#include <regex>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

ofstream f;
time_t currTime = time(NULL);

/** step_log
    Enters function stepping message in log file, with timestamp

    @param: s, Type string
    @return: void
*/
void step_log(string s)
{
    string x = ctime(&currTime);
    replace(x.begin(), x.end(), '\n', ' ');
    f << x << ":\tstep into function " << s << endl;
}

/** err_log
    Enters error message in log file, with timestamp

    @param: eCode, Type int
    @param: s, Type string
    @return: void
*/
void err_log(int eCode, string s)
{
    string x = ctime(&currTime);
    replace(x.begin(), x.end(), '\n', ' ');
    f << x << "\t:"
      << "Error Code " << eCode << ","<< s << endl;
}

//structure of a node in a tuple
struct node
{
    int data;
    string val = "";
    int type = 0;
};

//structure of columns
struct cols
{
    string name;
    int size;
    int type = 0;
    bool notNull = false;
    bool pk = false;
    bool unique = false;
    bool hasdef = false;
    string def_s = "";
    int def_i = 0;
};


vector<vector<vector<node *>>> db;
vector<vector<cols *>> col;
vector<string> table;
vector<pair<string, string>> al;
vector<int> select(vector<string>, int &, int, int);

/** newNode
    create new node  and set default values

    @param: s, Type string
    @return: node*
*/
node *newNode(string s)
{
    step_log("newNode");
    node *temp = new node;

    //if empty set string value  empty;type=string
    if (s == "")
    {
        temp->val = "null";
        temp->type = 1;
    }
    //if number, store it in temp->data
    else if (s[0] >= '0' && s[0] <= '9')
    {
        temp->data = stoi(s);
        temp->type = 0;
    }
    //else store in temp->val
    else
    {
        temp->val = s.substr(1, s.length() - 2);
        temp->type = 1;
    }
    return temp;
}

/** get_col_id
    returns column id from specified tab_id and column name

    @param: tab_id, Type int
    @param: s, Type string
    @return: int
*/
int get_col_id(int tab_id, string s)
{   
    step_log("get_col_id");
    for (int i = 0; i < col[tab_id].size(); i++)
    {
        if (col[tab_id][i]->name == s)
            return i;
    }
    return -1;
}

/** RELOP
    selects the row and column id to be printed based on relation operation (in form col_name op S) and returns it

    @param: tokens, Type vector<string>
    @param: table, Type int
    @param: i, Type &int
    @param: ptr2, Type int
    @return: vector<int>
*/
vector<int> RELOP(vector<string> tokens, int table_id, int &i, int ptr2)
{
	step_log("REL_OP");
	vector<cols *> ptable = col[table_id];	//get the current columns name from col and store it in ptable 						   
    vector<int> result;	//store the indexes of tuples which satisfy condn					
						
	
    vector<vector<node *>> rt = db[table_id];	//select the current table and store it in rt				

	//find the corresponding coloumn name
    for (int j = 0; j < ptable.size(); j++)
    {	
        if (ptable[j]->name == tokens[i]) //tokens[i] is the col_name
        {
            i++;
            string op = tokens[i];		//store the operator
            i++;

            string curr_tok = tokens[i];	//store the string/num to be compared
			
			//if the curr_tok is num, convert into type num and check the condn for each record           
			if (curr_tok[0] >= '0' && curr_tok[0] <= '9')
            {
                for (int k = 0; k < rt.size(); k++)
                {
                    if (op == "=")
                    {
                        if (rt[k][j]->data == stoi(curr_tok))
                            result.push_back(k);
                    }
                    else if (op == ">")
                    {
                        if (rt[k][j]->data > stoi(curr_tok))
                            result.push_back(k);
                    }
                    else if (op == "<")
                    {
                        if (rt[k][j]->data < stoi(curr_tok))
                            result.push_back(k);
                    }
                    else if (op == ">=")
                    {
                        if (rt[k][j]->data >= stoi(curr_tok))
                            result.push_back(k);
                    }
                    else if (op == "<=")
                    {
                        if (rt[k][j]->data <= stoi(curr_tok))
                            result.push_back(k);
                    }
                    else if (op == "!=")
                    {
                        if (rt[k][j]->data != stoi(curr_tok))
                            result.push_back(k);
                    }
                }
                break;
            }

			//if the curr_token is string
            else
            {
                curr_tok = curr_tok.substr(1, curr_tok.length() - 2);
                for (int k = 0; k < rt.size(); k++)
                {
                    if (rt[k][j]->val == curr_tok)
                        result.push_back(k);
                }
                break;
            }
            i++;		//increase the pointer to next token
        }
    }
    return result;	
}

/** where
    selects the row and column id to be printed based on conditions, and returns it

    @param: tokens, Type vector<string>, 
    @param: table, Type int
    @param: ptr, Type &int
    @param: ptr2, Type int
    @return: vector<int>
*/
vector<int> where(vector<string> tokens, int table, int &ptr, int ptr2)
{
    step_log("where");
    vector<int> result; //store the indexes of tuples which satisfy condn

	//get the current columns name from col and store it in ptable 
    vector<cols *> ptable = col[table];

	//select the current table and store it in rt
    vector<vector<node *>> rt = db[table];

	//go through each tokens after where statement till end ptr
    for (int i = ptr; i < ptr2; i++)
    {
        string s = tokens[i];	//store current token
		

		//if current token is "or", result = result U result2
        if (s == "or")
        {
            int sz = result.size(); //current size of result
            i++;
            vector<int> result2 = where(tokens, table, i, ptr2);	//find the result of condn after "or" and store in result2
            for (int k = 0; k < result2.size(); k++)
            {
                int flag = 1; 
                for (int l = 0; l < sz; l++)
                {
                    if (result[l] == result2[k]) //for same elements of result1 and result2, don't push in result
                    {
                        flag = 0;
                        break;
                    }
                }
                if (flag == 1)
                    result.push_back(result2[k]);	//for different elements of result1 and result2, push in result
            }
            return result;
        }
		

		//if current token is "and", result3 = result and result2
        else if (s == "and")
        {
            vector<int> result3;
            int sz = result.size();
            i++;
            vector<int> result2 = where(tokens, table, i, ptr2); //find the result of condn after "and" and store in result2
            for (int k = 0; k < result2.size(); k++)
            {
                for (int l = 0; l < sz; l++)
                {
                    if (result[l] == result2[k])
                    {
                        result3.push_back(result2[k]); // for elements which exists in result and result2, push_back in resul3
                        break;
                    }
                }
            }
            return result3;
        }
		

		// if current token == '(', again pass the input in where function with brackets removed
        else if (s == "(")
        {
            int error = 1;
            vector<int> result2;
            i++;
			//find closing bracket
            for (int j = i; j < ptr2; j++)
            {
                if (tokens[j] == ")")	//if closing bracket is found, pass the condn without brackets
                {
                    result2 = where(tokens, table, i, j); //store the result in result2 
                    error = 0;
                    i = j;
                    break;
                }
            }
			//push back in current result
            for (int k = 0; k < result2.size(); k++)
                result.push_back(result2[k]);

			//if ending bracket not found, print the error
            if (error)
                printf("\nERROR: UNMATCHED PARENTHESIS for %d th position '('\n", i + 1);
                err_log(15, "UNMATCHED PARENTHESIS for %d th position");
        }
		

		//if current token is comma, get to next token
        else if (s == ",")
        {
            continue;
        }

		//for remaining cases, ie. current_token= col_name 
        else
        {
			//in case of next token = "in"or "not in" 
			// col1 in ( select col2 from table ) || col1 in ( tuple) || col1 in ( select col2 from table where (condn)) 
            if (tokens[i + 1] == "in" || tokens[i + 1] == "not")
            {
                int cold = get_col_id(table, tokens[i]); //get coln index of col1
                int n_tabid, l; //n_tabid stores the tabid of the col2
				//find the ptr of ending bracket
                for (l = i + 2; l < ptr2; l++)
                {
                    if (tokens[l] == ")")
                        break;
                }
                int flag = tokens[i + 1] == "in";
                int beginptr = tokens[i + 1] == "in" ? i + 3 : i + 4; //points to select or tuple
                i = l;

				// col1 in ( select col2 from table ) || col1 in ( select col2 from table where (condn)) 
				if (tokens[beginptr] == "select")
                {
                    vector<int> temp = select(tokens, n_tabid, beginptr, l);
                    int n_colid = temp[temp.size() - 1]; //col_id of col2
                    
					//check type of coloumn 2; type=0 for int; type=1 for string;				
					int type = col[table][cold]->type;
                    for (int z = 0; z < db[table].size(); z++)
                    {
                        int flag2 = 0;
                        for (int m = 0; m < temp.size() - 2; m++)
                        {	
							//if type is int, check data attribute of both cols. if same, push_back in result
                            if (type == 0)
                            {
                                if ((db[n_tabid][temp[m]][n_colid]->data == db[table][z][cold]->data))
                                {
                                    flag2 = 1;
                                    if (flag)
                                        result.push_back(z);
                                    break;
                                }
                            }
							//if type is string, check string attribute of both cols. if same, push_back in result
                            else if (type == 1)
                            {
                                if ((db[n_tabid][temp[m]][n_colid]->val == db[table][z][cold]->val))
                                {
                                    flag2 = 1;
                                    if (flag)
                                        result.push_back(z);
                                    break;
                                }
                            }
                        }
                        if (!flag2 and !flag)
                            result.push_back(z);
                    }
                }


				// col1 in ( tuple)
                else
                {
                    vector<string> temp; //stores the tuple in string form
                    for (int z = beginptr; z < l; z = z + 2)
                    {
                        temp.push_back(tokens[z]);
                    }
					//checks type of col1; type=0 for int; type=1 for string
                    int type = col[table][cold]->type; 
	
                    for (int z = 0; z < db[table].size(); z++)
                    {
                        int flag2 = 0;	
                        for (int m = 0; m < temp.size(); m++)
                        {	
							//if type is int, check data attribute of both cols. if same, push_back in result
                            if (type == 0)
                            {
                                if (stoi(temp[m]) == db[table][z][cold]->data)
                                {
                                    flag2 = 1; //element found
									//for "in", push_back tuple index if element found
                                    if (flag)
                                        result.push_back(z);
                                    break;
                                }
                            }
							//if type is string, check string attribute of both cols. if same, push_back in result
                            else if (type == 1)
                            {
                                if (temp[m] == db[table][z][cold]->val)
                                {
                                    flag2 = 1;
                                    if (flag)
                                        result.push_back(z);
                                    break;
                                }
                            }
                        }
						//for "not in" flag=0; if not fount in tuple, then push_back in tuple
                        if (!flag2 and !flag)
                            result.push_back(z);
                    }
                }
            }
            else
                result = RELOP(tokens, table, i, ptr2); //else pass into relation operator; col1 op literal
        }
    }
    return result;
}

/** print
    prints specified attributes & tuples from tab_id

    @param: tb_id, Type int, contains the id of the table
    @param: rcid, Type vector<int> , contains rows and cols seperated by -1 to be printed
    @return void
*/
void print(int tb_id, vector<int> rcid)
{
    step_log("print");
    int i;
    //for table not exists print none;
    if (tb_id == -1)
        return;

    //find posn of -1
    for (i = 0; i < rcid.size(); i++)
    {
        if (rcid[i] == -1)
            break;
    }
    for (int p = 0; p < rcid.size() - (i + 1); p++)
        cout << "---------------------";
    cout << endl;

    //print the coloumn names
    for (int k = i + 1; k < rcid.size(); k++)
    {
        int flag = 1;
        for (int z = 0; z < al.size(); z++)
        {
            if (col[tb_id][rcid[k]]->name == al[z].first)

            {
                printf("| %-19s", al[z].second.c_str());
                flag = 0;
            }
        }
        if (flag)
            printf("| %-19s", col[tb_id][rcid[k]]->name.c_str());
    }
    cout << "|" << endl;
    for (int p = 0; p < rcid.size() - (i + 1); p++)
        cout << "---------------------";
    cout << endl;

    //print the rows and coloums from database which are in rcid
    for (int j = 0; j < i; j++)
    {
        for (int k = i + 1; k < rcid.size(); k++)
        {
            if (db[tb_id][rcid[j]][rcid[k]]->val != "")
                printf("| %-19s", db[tb_id][rcid[j]][rcid[k]]->val.c_str());
            else
                printf("| %-19d", db[tb_id][rcid[j]][rcid[k]]->data);
        }
        cout << "|" << endl;
    }
    for (int p = 0; p < rcid.size() - (i + 1); p++)
        cout << "---------------------";
    cout << endl;
}

/** find_tab_id
    Returns table id from database

    @param s Type string
    @return int
*/
int find_tab_id(string s)
{
    step_log("find_tab_id");
    for (int i = 0; i < table.size(); i++)
    {
        if (table[i] == s)
            return i;
    }
    return -1;
}

/** create
    Creates a new table in the database

    @param tokens Type vector<string>
    @return void
*/
void create(vector<string> tokens)
{
    step_log("create");
    if (find_tab_id(tokens[2]) != -1)
    {
        cout << "\nERROR:Table " << tokens[2] << " already exists in database.\n";
        err_log(1, "Table already exists in database");
        return;
    }
    table.push_back(tokens[2]); //push back table name in table
    int j;
    vector<cols *> tmp;
    for (int i = 4; i < tokens.size() - 1; i++)
    {
        cols *temp = new cols();
        //for each temp, create a new col and set the arrtibutes of temp accordingly till comma is found
        for (j = i; tokens[j] != "," && j < tokens.size() - 1; j++)
        {
            if (tokens[j] == "not" && tokens[j + 1] == "null")
            {
                temp->notNull = true;
                j++;
            }

            //check for unique keyword and change the attribute
            else if (tokens[j] == "unique")
                temp->unique = true;
            
            //check for primary key keyword
            else if (tokens[j] == "primary" && tokens[j + 1] == "key")
            {
                temp->pk = true;
                j++;
            }

            //check for default keyword 
            else if (tokens[j] == "default")
            {   
                //if next token is num
                if (tokens[j + 1][0] >= '0' && tokens[j + 1][0] <= '9')
                    temp->def_i = stoi(tokens[j + 1]);
                //if next token is string
                else
                {
                    temp->def_s = tokens[j + 1].substr(1, tokens[j + 1].length() - 2);
                    temp->type = 1;
                }
                temp->hasdef = true;
                j++;
            }

            //if varchar set type=1
            else if (tokens[j] == "varchar")
            {
                temp->type = 1;
            }
            //if int set type=0 
            else if (tokens[j] == "int")
            {
                temp->type = 0;
            }
            else if (tokens[j] == "(")
            {
                temp->size = stoi(tokens[j + 1]);
                j += 2;
            }
            //else set col name 
            else
            {
                temp->name = tokens[j];
            }
        }
        i = j;
        tmp.push_back(temp); 
    }
    col.push_back(tmp); //push_back in coloumns
}

/** select
    Selects the attributes and tuples from table

    @param: tokens Type vector<string>
    @param: tab_id, Type int&
    @param ptr Type int
    @param: endptr Type int
    @return: vector<int>
*/
vector<int> select(vector<string> tokens, int &tab_id, int ptr, int endptr)
{
    step_log("select");
    int i, save_i;

    vector<int> result;	//result will contains row_ids satisfying condn, after that -1, after that col_ids 

	//find the position of "from" and store it in i
    for (i = ptr; i < endptr; i++)
    {
        if (tokens[i] == "from")
            break;
    }
	
	//for invalid syntax, generates error
    if (i == endptr || i + 1 == endptr)
    {   
         err_log(2, "Invalid Syntax");
        cout << "\nERROR: INVALID SYNTAX\n";
        return result;
    }
    if (tokens[i + 1] != "(")
    {
        tab_id = find_tab_id(tokens[i + 1]); //find table_id
        if (tab_id == -1)
        {
            cout << "\nERROR:Table " << tokens[i + 1] << " doesn't exist in database\n";
            err_log(3, "Table doesn't exist int the database");
            vector<int> t = {-1};
            return t;
        }
        else
        {
            save_i = i; //stores index of col1
			//checks for where ie select (col1,col2..) from table where (condn); 	            
			if (i + 2 != endptr)
            {
                if (tokens[i + 2] != "where")
                {
                    printf("\nERROR: where not found");
                    err_log(10, "Missing \'where\' token");
                    return result;
                }
                i = i + 3; 
                vector<int> rowids = where(tokens, tab_id, i, endptr); //stores index of tuple satisfy where condn
                for (int j = 0; j < rowids.size(); j++)
                    result.push_back(rowids[j]);
                result.push_back(-1);
            }
			
			//if not where found, then push all tuples
            else
            {
                for (int j = 0; j < db[tab_id].size(); j++)
                    result.push_back(j);
                result.push_back(-1);
            }
			//if * found after select, push all cols
            if (tokens[ptr + 1] == "*")
            {
                for (int j = 0; j < col[tab_id].size(); j++)
                {
                    result.push_back(j);
                }
            }
			//else push only selected cols
            else
            {
                for (int j = ptr + 1; j < save_i; j = j + 2)
                {
                    int r = get_col_id(tab_id, tokens[j]);
                    result.push_back(r);
                }
            }
        }
        return result;
    }
	//for nested queries like select col1 from (select * from table1); 
    else
    {
        int tab_id = -1;
        vector<int> result2;
        if (tokens[endptr - 1] != ")")
        {
            printf("\nERROR: UNMATCHED PARENTHESIS\n");
            err_log(5, "Unmatched Parentheses");
            result2.push_back(-1);
            return result2;
        }
		//stores result of nested query
        result2 = select(tokens, tab_id, i + 1, endptr - 1);
        if (tab_id == -1)
        {
            cout << "\nERROR:Table " << tokens[i + 1] << " doesn't exist in database\n";
            err_log(3, "Table doesn't exist in the database");
            vector<int> t = {-1};
            return t;
        }

        int k;
		//find -1 in result2
        for (k = 0; k < result2.size(); k++)
        {
            if (result2[k] = -1)
                break;
            result.push_back(result2[k]);
        }
        result.push_back(-1);

		//push all cols if "*" is found
        if (tokens[ptr + 1] == "*")
        {
            for (int l = k + 1; l < result2.size(); l++)
            {
                result.push_back(result2[l]);
            }
        }
		//else push only given cols
        else
        {
            for (int j = ptr + 1; j < save_i; j = j + 2)
            {
                int r = get_col_id(tab_id, tokens[j]);
                for (int l = k + 1; l < result2.size(); l++)
                {
                    if (result2[l] == r)
                        result.push_back(k);
                    break;
                }
            }
        }
    }
}

/** insert
    inserts a tuple into specified table in database

    @param tokens Type vector<string>
    @return void
*/
void insert(vector<string> tokens)
{
    step_log("insert");
    int tab_id = find_tab_id(tokens[2]);
    //if table not found generate
    if (tab_id == -1)
    {
        cout << "\nERROR: Table " << tokens[2] << " doesn't exist in the database\n";
        err_log(3, "Table doesn't exist in the database");
        return;
    }
    //if string= insert into table values ( value1, value2..)
    else if (tokens[3] == "values")
    {
        vector<vector<node *>> curr; 
        vector<node *> tmp; // make new node for each value
        for (int i = 5; i < tokens.size() - 1; i += 2)
        {
            node *temp = newNode(tokens[i]);
            tmp.push_back(temp);
        }
        //if not first record, insert record in corresponding col in databse
        if (table.size() == db.size())
            db[tab_id].push_back(tmp);
        //for first record, insert a whole array of rows and coloumn curr
        else
        {
            curr.push_back(tmp);
            db.push_back(curr);
        }
    }

    //if string= insert into table (col1,col2..)values ( value1, value2..)
    else
    {
        vector<pair<int, node *>> tmp;
        int j;
        for (j = 4; j < tokens.size(); j++)
        {
            if (tokens[j] == "(")
            {
                j++;
                break;
            }
        }
        //create array of ids of col1,col2.. with name  and make new nodes also for values
        for (int i = 4; tokens[i] != ")" && tokens[i] != "values"; i += 2, j += 2)
            tmp.push_back(make_pair(get_col_id(tab_id, tokens[i]), newNode(tokens[j])));
        
        sort(tmp.begin(), tmp.end());
        //generates error for invalid coloumns
        if (tmp[0].first == -1)
        {
            err_log(12, "One or more column names are invalid");
            cout << "\nERROR: One or more column names is/are invalid.\n";
            return;
        }
        int curr = 0;
        vector<node *> n;

        //for each col id, check constraints
        for (int i = 0; i < col[tab_id].size(); i++)
        {   
            //checking for aliases 
            if (curr < tmp.size() && tmp[curr].first == i)
            {
                n.push_back(tmp[curr].second);
                curr++;
            }
            else
            {
                if (!col[tab_id][i]->hasdef)
                {
                    if (col[tab_id][i]->notNull)
                    {
                        cout << "\nERROR: Field \'" << col[tab_id][i]->name << "\' doesn't have a default value & it can't be null.\n";
                        err_log(17, "Fields without default value and notNull=true can't be skipped while inserting");
                        return;
                    }
                }
                if (col[tab_id][i]->type == 0)
                    n.push_back(newNode(to_string(col[tab_id][i]->def_i)));
                else
                    n.push_back(newNode(col[tab_id][i]->def_s));
            }
        }
        //if not first entry, directly push_bak in db
        if (table.size() == db.size())
            db[tab_id].push_back(n);
        //else create a new vector curr and push it in db
        else
        {
            vector<vector<node *>> curr;
            curr.push_back(n);
            db.push_back(curr);
        }
    }
}


/** Drop
    Deletes a table from the database

    @param tokens Type vector<string>
    @return void
*/
void drop(vector<string> tokens)
{
    step_log("drop");
    int tab_id = find_tab_id(tokens[2]);
    //generate error if table not founf
    if (tab_id == -1)
    {
        cout << "\nERROR: Table " << tokens[2] << " doesn't exist in the database\n";
        err_log(3, "Table doesn't exist in the database");
        return;
    }
    //deletes table from table vector,all tuples in database and corresponding coloumns in col
    table[tab_id] = table[table.size() - 1];
    table.pop_back();
    db[tab_id] = db[db.size() - 1];
    db.pop_back();
    col[tab_id] = col[col.size() - 1];
    col.pop_back();
}

/** Del
    Drop a table from the database

    @param tokens Type vector<string>
    @return void
*/
void del(vector<string> tokens)
{
    step_log("del");
    int i = 2;

    if (i == tokens.size() || i + 1 == tokens.size())
    {
        cout << "\nERROR: INVALID SYNTAX\n";
        return;
    }
    int tab_id = find_tab_id(tokens[i]);

    //if table not found show error
    if (tab_id == -1)
    {
        cout << "\nERROR:Table doesn't exist in database\n";
        err_log(3, "Table doesn't exist in the database");
        return;
    }
    else
    {
        i = i + 2;
        //find corresponding rowids which need to be deleted from where condn
        vector<int> rowids = where(tokens, tab_id, i, tokens.size());
        vector<vector<node *>> copytb;
        //create copydb and push records which need not to be deleted
        for (int i = 0; i < db[tab_id].size(); i++)
        {
            int flag = 1;
            for (int j = 0; j < rowids.size(); j++)
            {
                if (rowids[j] == i)
                {
                    flag = 0;
                    break;
                }
            }
            if (flag == 1)
                copytb.push_back(db[tab_id][i]);
        }
        //set  pointer that new copytb
        db[tab_id] = copytb;
    }

    return;
}

int main()
{
    string s;
    f.open("log.txt", ofstream::out | ofstream::app);
    vector<string> t;
    printf(">> ");
    getline(cin, s, ';');
    string x = ctime(&currTime);
    f <<"\n"<< x << "Input Query: " << s << endl;
    if (!isatty(fileno(stdin)))
    {
        printf("%s\n", s.c_str());
    }
    //run the loop till exit command is not typed
    while (s != "exit")
    {
        replace(s.begin(), s.end(), '\n', ' ');
        string p;
        for (int i = 0; i < s.length(); i++)
        {
            char ch = s[i];
            if (i < s.length() - 1)
            {
                if (ch == ',' || ch == '(' || ch == ')' || (ch == '<' && s[i + 1] != '=') || (ch == '>' && s[i + 1] != '=') || ch == '=')
                {
                    p = p + ' ' + ch + ' ';
                }
                else if ((ch == '<' && s[i + 1] == '=') || (ch == '>' && s[i + 1] == '=') || (ch == '!' && s[i + 1] == '='))
                {
                    p = p + ' ' + ch + '=' + ' ';
                }
                else
                    p = p + ch;
            }
            else
            {
                if (ch == ')')
                {
                    p = p + ' ' + ch + ' ';
                }
                else
                    p = p + ch;
            }
        }
        s = p;
        t.resize(0);
        regex e("[ ]+");
        regex_token_iterator<string::iterator> i(s.begin(), s.end(), e, -1);
        regex_token_iterator<string::iterator> end;
        while (i != end)
            t.push_back(*i++);
        s = "";
        for (int i = 0; i < t.size(); i++)
            s = s + t[i] + " ";

        stringstream check1(s);
        string intermediate;
        vector<string> tokens;
        while (getline(check1, intermediate, ' '))
            tokens.push_back(intermediate);

        //resetting alias vector
        al.resize(0);
        //inserting values in alias vector as pair of "original name", "alias name"
        for (int i = 0; i < tokens.size() - 1; i++)
        {
            if (tokens[i] == "as")
                al.push_back(make_pair(tokens[i - 1], tokens[i + 1]));
        }
        vector<string> tok;
        for (int i = 0; i < tokens.size(); i++)
        {
            if (tokens[i] == "as")
            {
                i++;
                continue;
            }
            int flag = 1;
            for (int j = 0; j < al.size(); j++)
            {
                if (tokens[i] == al[j].second)
                {
                    tok.push_back(al[j].first);
                    flag = 0;
                }
            }
            if (flag)
                tok.push_back(tokens[i]);
        }
        tokens.resize(0);
        for (int i = 0; i < tok.size(); i++)
        {
            string s = tok[i];
            transform(s.begin(), s.end(), s.begin(), ::tolower);
            if (s == "create" || s == "select" || s == "insert" || s == "delete" || s == "update" || s == "drop" || s == "alter" || s == "delete" || s == "in" || s == "not" || s == "null" || s == "primary" || s == "key" || s == "foreign" || s == "default" || s == "unique" || s == "int" || s == "varchar" || s == "from" || s == "where" || s == "table" || s == "alter" || s == "exit")
            {
                tokens.push_back(s);
            }
            else
                tokens.push_back(tok[i]);
        }

        //Goto relevant function based on first token in the query
        if (tokens[0] == "create")
            create(tokens);
        if (tokens[0] == "select")
        {
            int tab_id = -1;
            vector<int> result = select(tokens, tab_id, 0, tokens.size());
            print(tab_id, result);
        }
        if (tokens[0] == "insert")
            insert(tokens);
        if (tokens[0] == "delete")
            del(tokens);
        if (tokens[0] == "drop")
            drop(tokens);
        getline(cin, s);
        getline(cin, s, ';');           //get next query
        string x = ctime(&currTime);
        f <<"\n"<< x << "Input Query: " << s << endl;
        printf("\n>> ");                //print prompt on terminal
        //print the query onto terminal if read from a file
        if (!isatty(fileno(stdin)))
        {
            printf("%s\n", s.c_str());
        }
    }
    return 0;
}
