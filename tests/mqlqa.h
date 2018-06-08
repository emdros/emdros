class QueryAnswer {
      public:
      QueryAnswer(bool bNewDB,
                  const char *query_name, 
                  const char *query, 
                  bool bDBSuccessExpected,
                  bool bCompilerSuccessExpected,
		  const char *answer) {
		  m_bNewDB = bNewDB;
		  m_query_name = query_name;
		  m_query = query;
		  m_bDBSuccessExpected = bDBSuccessExpected;
		  m_bCompilerSuccessExpected = bCompilerSuccessExpected;
		  m_answer = answer;
      };
      std::string m_query_name, m_query, m_answer;
      bool m_bNewDB;
      bool m_bDBSuccessExpected, m_bCompilerSuccessExpected;
      };
QueryAnswer mqlqa[] = {


//CREATING Token object type
QueryAnswer(true, "CREATING Token object type",
  "CREATE ENUMERATION boolean_t = {\n"
  "   false = 0,\n"
  "   true\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE MONAD OBJECTS\n"
  "[Token\n"
  "	surface : STRING DEFAULT \"boo\";\n"
  "	surface_set : STRING FROM SET DEFAULT \"blof\";\n"
  "	surface_a : ASCII DEFAULT \"blah\";\n"
  "	surface_a_set : STRING FROM SET DEFAULT \"baf\";\n"
  "	myint : INTEGER DEFAULT -1;\n"
  "	myint2 : INTEGER DEFAULT 2;\n"
  "	boolean : boolean_t;\n"
  "	bool_list : LIST OF BOOLEAN_T;\n"
  "	Parent : id_d;\n"
  "	Parents : LIST OF ID_D;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECTS WITH OBJECT TYPE [Token]\n"
  "CREATE OBJECT FROM MONADS = {1}\n"
  "WITH ID_D = 1\n"
  "[]\n"
  "CREATE OBJECT FROM MONADS = {2}\n"
  "WITH ID_D = 2\n"
  "[\n"
  "	parents := (1,1);\n"
  "	parent := 1;\n"
  "	bool_list := (false, true);\n"
  "	boolean := true;\n"
  "	myint2 := 3;\n"
  "	myint := 4;\n"
  "	surface_a_set := \"\";\n"
  "	surface_a := \"\";\n"
  "	surface_set := \"b\";\n"
  "	surface := \"b\";\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {3}\n"
  "WITH ID_D = 3\n"
  "[\n"
  "	parents := (2,1);\n"
  "	parent := 2;\n"
  "	bool_list := (true);\n"
  "	boolean := true;\n"
  "	surface_a_set := \"c\";\n"
  "	surface_a := \"d\";\n"
  "	surface_set := \"a\";\n"
  "	surface := \"f\";\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 3                      |\n"
  "+------------------------+\n"
  ""),


//querying just created tokens
QueryAnswer(false, "querying just created tokens",
  "GET OBJECTS HAVING MONADS IN {1-3}[Token GET ALL] GO\n"
  "",
  true, true,
  " // <  < [ Token 1 { 1 } false (parents=\"\",parent=\"0\",bool_list=\"\",boolean=\"false\",myint2=\"2\",myint=\"-1\",surface_a_set=\"baf\",surface_a=\"blah\",surface_set=\"blof\",surface=\"boo\",self=\"1\") //  <  > \n"
  " ]\n"
  "[ Token 2 { 2 } false (parents=\" 1 1 \",parent=\"1\",bool_list=\" false true \",boolean=\"true\",myint2=\"3\",myint=\"4\",surface_a_set=\"\",surface_a=\"\",surface_set=\"b\",surface=\"b\",self=\"2\") //  <  > \n"
  " ]\n"
  "[ Token 3 { 3 } false (parents=\" 2 1 \",parent=\"2\",bool_list=\" true \",boolean=\"true\",myint2=\"2\",myint=\"-1\",surface_a_set=\"c\",surface_a=\"d\",surface_set=\"a\",surface=\"f\",self=\"3\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//creating realistic DB schema
QueryAnswer(true, "creating realistic DB schema",
  "CREATE ENUMERATION pos_t = {\n"
  "	def_article, noun, verb, relative_pronoun, preposition, adjective\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE ENUMERATION phrase_type_t = {\n"
  "   NP, VP, PP, AP\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE ENUMERATION phrase_function_t = {\n"
  "   NA, Subj, Objc, Rela, Pred, Loca, PreC, Attr\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE MONAD OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[Word\n"
  "   surface: STRING FROM SET;\n"
  "   pos : pos_t;\n"
  "   parent : id_d;\n"
  "   lemma : STRING FROM SET WITH INDEX;\n"
  "   parents : LIST OF id_d;\n"
  "   magic : integer DEFAULT 42;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH MULTIPLE RANGE OBJECTS\n"
  "[Phrase\n"
  "  phrase_type : phrase_type_t;\n"
  "  phrase_function : LIST OF phrase_function_t;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[Clause_atom]\n"
  "GO\n"
  "\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH MULTIPLE RANGE OBJECTS\n"
  "[Clause]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH MULTIPLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[Sentence]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST AND LAST MONADS\n"
  "[Paragraph]\n"
  "GO\n"
  "",
  true, true,
  ""),


//creating realistic Word objects
QueryAnswer(false, "creating realistic Word objects",
  "CREATE OBJECTS WITH OBJECT TYPE [Word]\n"
  "CREATE OBJECT FROM MONADS = {1}\n"
  "WITH ID_D = 1\n"
  "[\n"
  "   surface :=\"The\";\n"
  "   pos := def_article;\n"
  "   parent := 10;\n"
  "   lemma := \"the\";\n"
  "   parents := (10,17,20,22,23);\n"
  "   magic := 2;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {2}\n"
  "WITH ID_D = 2\n"
  "[\n"
  "   surface :=\"door,\";\n"
  "   pos := noun;\n"
  "   parent := 10;\n"
  "   lemma := \"door\";\n"
  "   parents := (10,17,20,22,23);\n"
  "   magic := 1;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {3}\n"
  "WITH ID_D = 3\n"
  "[\n"
  "   surface :=\"which\";\n"
  "   pos := relative_pronoun;\n"
  "   parent := 11;\n"
  "   lemma := \"which\";\n"
  "   parents := (11,18,21,22,23);\n"
  "   magic := 3;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {4}\n"
  "WITH ID_D = 4\n"
  "[\n"
  "   surface :=\"opened\";\n"
  "   pos := verb;\n"
  "   parent := 12;\n"
  "   lemma := \"open\";\n"
  "   parents := (12,18,21,22,23);\n"
  "   magic := 7;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {5}\n"
  "WITH ID_D = 5\n"
  "[\n"
  "   surface :=\"towards\";\n"
  "   pos := preposition;\n"
  "   parent := 13;\n"
  "   lemma := \"towards\";\n"
  "   parents := (13,18,21,22,23);\n"
  "   magic := 7;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {6}\n"
  "WITH ID_D = 6\n"
  "[\n"
  "   surface :=\"the\";\n"
  "   pos := def_article;\n"
  "   parent := 14;\n"
  "   lemma := \"the\";\n"
  "   parents := (14,18,21,22,23);\n"
  "   magic := 7;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {7}\n"
  "WITH ID_D = 7\n"
  "[\n"
  "   surface :=\"East,\";\n"
  "   pos := noun;\n"
  "   parent := 14;\n"
  "   lemma := \"east\";\n"
  "   parents := (14,18,21,22,23);\n"
  "   magic := 9;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {8}\n"
  "WITH ID_D = 8\n"
  "[\n"
  "   surface :=\"was\";\n"
  "   pos := verb;\n"
  "   parent := 15;\n"
  "   lemma := \"be\";\n"
  "   parents := (15,19,20,22,23);\n"
  "   magic := 6;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {9}\n"
  "WITH ID_D = 9\n"
  "[\n"
  "   surface :=\"blue.\";\n"
  "   pos := adjective;\n"
  "   parent := 16;\n"
  "   lemma := \"blue\";\n"
  "   parents := (16,19,20,22,23);\n"
  "   magic := 11;\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 9                      |\n"
  "+------------------------+\n"
  ""),


//creating realistic Phrase objects
QueryAnswer(false, "creating realistic Phrase objects",
  "CREATE OBJECTS WITH OBJECT TYPE [Phrase]\n"
  "CREATE OBJECT FROM MONADS = {1-2}\n"
  "WITH ID_D = 10\n"
  "[\n"
  "  phrase_type := NP;\n"
  "  phrase_function := (Subj);\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {3-3}\n"
  "WITH ID_D = 11\n"
  "[\n"
  "  phrase_type := NP;\n"
  "  phrase_function := (Rela,Subj);\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {4-4}\n"
  "WITH ID_D = 12\n"
  "[\n"
  "  phrase_type := VP;\n"
  "  phrase_function := (Pred);\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {6-7}\n"
  "WITH ID_D = 14\n"
  "[\n"
  "  phrase_type := NP;\n"
  "  phrase_function := (NA);\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {5-7}\n"
  "WITH ID_D = 13\n"
  "[\n"
  "  phrase_type := PP;\n"
  "  phrase_function := (Loca);\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {8-8}\n"
  "WITH ID_D = 15\n"
  "[\n"
  "  phrase_type := VP;\n"
  "  phrase_function := (PreC);\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {9}\n"
  "WITH ID_D = 16\n"
  "[\n"
  "  phrase_type := AP;\n"
  "  phrase_function := (Attr);\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 7                      |\n"
  "+------------------------+\n"
  ""),


//creating realistic Clause_atom objects
QueryAnswer(false, "creating realistic Clause_atom objects",
  "CREATE OBJECTS WITH OBJECT TYPE [Clause_atom]\n"
  "CREATE OBJECT FROM MONADS = {1-2}\n"
  "WITH ID_D = 17\n"
  "[]\n"
  "CREATE OBJECT FROM MONADS = {3-7}\n"
  "WITH ID_D = 18\n"
  "[]\n"
  "CREATE OBJECT FROM MONADS = {8-9}\n"
  "WITH ID_D = 19\n"
  "[]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 3                      |\n"
  "+------------------------+\n"
  ""),


//creating realistic Clause objects
QueryAnswer(false, "creating realistic Clause objects",
  "CREATE OBJECTS WITH OBJECT TYPE [Clause]\n"
  "CREATE OBJECT FROM MONADS = {1-2,8-9}\n"
  "WITH ID_D = 20\n"
  "[]\n"
  "CREATE OBJECT FROM MONADS = {3-7}\n"
  "WITH ID_D = 21\n"
  "[]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 2                      |\n"
  "+------------------------+\n"
  ""),


//creating realistic Sentence object
QueryAnswer(false, "creating realistic Sentence object",
  "CREATE OBJECT FROM MONADS = {1-9}\n"
  "WITH ID_D = 22\n"
  "[Sentence]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 22          |\n"
  "+-------------+\n"
  ""),


//creating realistic Paragraph object
QueryAnswer(false, "creating realistic Paragraph object",
  "CREATE OBJECT FROM MONADS = {1-9}\n"
  "WITH ID_D = 23\n"
  "[Paragraph]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 23          |\n"
  "+-------------+\n"
  ""),


//selecting object types
QueryAnswer(false, "selecting object types",
  "SELECT OBJECT TYPES GO\n"
  "",
  true, true,
  "+----------------------+\n"
  "| object type : string |\n"
  "+----------------------+\n"
  "| word                 |\n"
  "| phrase               |\n"
  "| clause_atom          |\n"
  "| clause               |\n"
  "| sentence             |\n"
  "| paragraph            |\n"
  "+----------------------+\n"
  ""),


//selecting features from object type
QueryAnswer(false, "selecting features from object type",
  "SELECT FEATURES FROM OBJECT TYPE [Token] GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  ""),


//selecting features from object type
QueryAnswer(false, "selecting features from object type",
  "SELECT FEATURES FROM OBJECT TYPE [Word] GO\n"
  "",
  true, true,
  "+-----------------------+---------------+------------------+-----------------+\n"
  "| feature name : string | type : string | default : string | computed : bool |\n"
  "+-----------------------+---------------+------------------+-----------------+\n"
  "| magic                 | integer       | '42'             | false           |\n"
  "| parents               | list of id_d  | ''               | false           |\n"
  "| lemma                 | string        | ''               | false           |\n"
  "| parent                | id_d          | '0'              | false           |\n"
  "| pos                   | pos_t         | 'def_article'    | false           |\n"
  "| surface               | string        | ''               | false           |\n"
  "| self                  | id_d          | '0'              | true            |\n"
  "+-----------------------+---------------+------------------+-----------------+\n"
  ""),


//selecting enumerations
QueryAnswer(false, "selecting enumerations",
  "SELECT ENUMERATIONS GO\n"
  "",
  true, true,
  "+--------------------+\n"
  "| enum name : string |\n"
  "+--------------------+\n"
  "| pos_t              |\n"
  "| phrase_type_t      |\n"
  "| phrase_function_t  |\n"
  "+--------------------+\n"
  ""),


//selecting enumeration constants
QueryAnswer(false, "selecting enumeration constants",
  "SELECT ENUMERATION CONSTANTS \n"
  "FROM ENUMERATION phrase_types_t \n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration phrase_types_t did not exist.\n"
  ""),


//selecting enumeration constants
QueryAnswer(false, "selecting enumeration constants",
  "SELECT ENUMERATION CONSTANTS \n"
  "FROM ENUMERATION phrase_type_t \n"
  "GO\n"
  "",
  true, true,
  "+---------------------+-----------------+----------------+\n"
  "| enum const : string | value : integer | default : bool |\n"
  "+---------------------+-----------------+----------------+\n"
  "| AP                  | 4               | false          |\n"
  "| NP                  | 1               | true           |\n"
  "| PP                  | 3               | false          |\n"
  "| VP                  | 2               | false          |\n"
  "+---------------------+-----------------+----------------+\n"
  ""),


//selecting object types using enumerations
QueryAnswer(false, "selecting object types using enumerations",
  "SELECT OBJECT TYPES \n"
  "USING ENUMERATION pos_t GO\n"
  "",
  true, true,
  "+---------------------------+\n"
  "| object type name : string |\n"
  "+---------------------------+\n"
  "| word                      |\n"
  "+---------------------------+\n"
  ""),


//selecting min_m
QueryAnswer(false, "selecting min_m",
  "SELECT min_m GO\n"
  "",
  true, true,
  "+-----------------+\n"
  "| min_m : monad_m |\n"
  "+-----------------+\n"
  "| 1               |\n"
  "+-----------------+\n"
  ""),


//selecting max_m
QueryAnswer(false, "selecting max_m",
  "SELECT max_m GO\n"
  "",
  true, true,
  "+-----------------+\n"
  "| max_m : monad_m |\n"
  "+-----------------+\n"
  "| 9               |\n"
  "+-----------------+\n"
  ""),


//selecting monad sets when there are none
QueryAnswer(false, "selecting monad sets when there are none",
  "SELECT MONAD SETS GO\n"
  "",
  true, true,
  "+-------------------------+\n"
  "| monad_set_name : string |\n"
  "+-------------------------+\n"
  "+-------------------------+\n"
  ""),


//getting monad sets when there are none
QueryAnswer(false, "getting monad sets when there are none",
  "GET MONAD SETS ALL GO\n"
  "",
  true, true,
  "+-------------------------+---------------------+--------------------+\n"
  "| monad_set_name : string | mse_first : monad_m | mse_last : monad_m |\n"
  "+-------------------------+---------------------+--------------------+\n"
  "+-------------------------+---------------------+--------------------+\n"
  ""),


//getting monads from clause object with id_ds
QueryAnswer(false, "getting monads from clause object with id_ds",
  "GET MONADS FROM OBJECTS\n"
  "WITH ID_DS = 20, 21\n"
  "[Clause]\n"
  "GO\n"
  "",
  true, true,
  "+--------------------+---------------------+--------------------+\n"
  "| object_id_d : id_d | mse_first : monad_m | mse_last : monad_m |\n"
  "+--------------------+---------------------+--------------------+\n"
  "| 20                 | 1                   | 2                  |\n"
  "| 20                 | 8                   | 9                  |\n"
  "| 21                 | 3                   | 7                  |\n"
  "+--------------------+---------------------+--------------------+\n"
  ""),


//getting monads from phrase object with id_ds
QueryAnswer(false, "getting monads from phrase object with id_ds",
  "GET MONADS FROM OBJECTS\n"
  "WITH ID_DS = 10, 13, 14, 15, 16\n"
  "[Phrase]\n"
  "GO\n"
  "",
  true, true,
  "+--------------------+---------------------+--------------------+\n"
  "| object_id_d : id_d | mse_first : monad_m | mse_last : monad_m |\n"
  "+--------------------+---------------------+--------------------+\n"
  "| 10                 | 1                   | 2                  |\n"
  "| 13                 | 5                   | 7                  |\n"
  "| 14                 | 6                   | 7                  |\n"
  "| 15                 | 8                   | 8                  |\n"
  "| 16                 | 9                   | 9                  |\n"
  "+--------------------+---------------------+--------------------+\n"
  ""),


//getting monads from word object with id_ds
QueryAnswer(false, "getting monads from word object with id_ds",
  "GET MONADS FROM OBJECTS\n"
  "WITH ID_DS = 1, 3, 4, 7, 9\n"
  "[Word]\n"
  "GO\n"
  "",
  true, true,
  "+--------------------+---------------------+--------------------+\n"
  "| object_id_d : id_d | mse_first : monad_m | mse_last : monad_m |\n"
  "+--------------------+---------------------+--------------------+\n"
  "| 1                  | 1                   | 1                  |\n"
  "| 3                  | 3                   | 3                  |\n"
  "| 4                  | 4                   | 4                  |\n"
  "| 7                  | 7                   | 7                  |\n"
  "| 9                  | 9                   | 9                  |\n"
  "+--------------------+---------------------+--------------------+\n"
  ""),


//getting monads from sentence object with id_ds
QueryAnswer(false, "getting monads from sentence object with id_ds",
  "GET MONADS FROM OBJECTS\n"
  "WITH ID_DS = 22\n"
  "[Sentence]\n"
  "GO\n"
  "",
  true, true,
  "+--------------------+---------------------+--------------------+\n"
  "| object_id_d : id_d | mse_first : monad_m | mse_last : monad_m |\n"
  "+--------------------+---------------------+--------------------+\n"
  "| 22                 | 1                   | 9                  |\n"
  "+--------------------+---------------------+--------------------+\n"
  ""),


//getting monads from paragraph object with id_ds
QueryAnswer(false, "getting monads from paragraph object with id_ds",
  "GET MONADS FROM OBJECTS\n"
  "WITH ID_DS = 23\n"
  "[Paragraph]\n"
  "GO\n"
  "",
  true, true,
  "+--------------------+---------------------+--------------------+\n"
  "| object_id_d : id_d | mse_first : monad_m | mse_last : monad_m |\n"
  "+--------------------+---------------------+--------------------+\n"
  "| 23                 | 1                   | 9                  |\n"
  "+--------------------+---------------------+--------------------+\n"
  ""),


//getting features from word object 1
QueryAnswer(false, "getting features from word object 1",
  "GET FEATURES surface, pos, parent\n"
  "FROM OBJECTS WITH ID_DS = 2,5\n"
  "[Word]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+------------------+-------------------+---------------+\n"
  "| id_d : id_d | surface : string | pos : enum(pos_t) | parent : id_d |\n"
  "+-------------+------------------+-------------------+---------------+\n"
  "| 2           | door,            | noun              | 10            |\n"
  "| 5           | towards          | preposition       | 13            |\n"
  "+-------------+------------------+-------------------+---------------+\n"
  ""),


//getting features from word object 2
QueryAnswer(false, "getting features from word object 2",
  "GET FEATURES surface, pos, parent\n"
  "FROM OBJECTS WITH ID_DS = 1,2,2,2,3,3,4\n"
  "[Word]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+------------------+-------------------+---------------+\n"
  "| id_d : id_d | surface : string | pos : enum(pos_t) | parent : id_d |\n"
  "+-------------+------------------+-------------------+---------------+\n"
  "| 1           | The              | def_article       | 10            |\n"
  "| 2           | door,            | noun              | 10            |\n"
  "| 3           | which            | relative_pronoun  | 11            |\n"
  "| 4           | opened           | verb              | 12            |\n"
  "+-------------+------------------+-------------------+---------------+\n"
  ""),


//getting features from word object 2.1
QueryAnswer(false, "getting features from word object 2.1",
  "GET FEATURES surface, pos, parent\n"
  "FROM OBJECTS WITH ID_DS = 4,3,3,2,2,2,2,1\n"
  "[Word]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+------------------+-------------------+---------------+\n"
  "| id_d : id_d | surface : string | pos : enum(pos_t) | parent : id_d |\n"
  "+-------------+------------------+-------------------+---------------+\n"
  "| 1           | The              | def_article       | 10            |\n"
  "| 2           | door,            | noun              | 10            |\n"
  "| 3           | which            | relative_pronoun  | 11            |\n"
  "| 4           | opened           | verb              | 12            |\n"
  "+-------------+------------------+-------------------+---------------+\n"
  ""),


//getting features from word object 3
QueryAnswer(false, "getting features from word object 3",
  "GET FEATURES parent, pos\n"
  "FROM OBJECTS WITH ID_DS = 1,2,2,2,3,3,4\n"
  "[Word]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+---------------+-------------------+\n"
  "| id_d : id_d | parent : id_d | pos : enum(pos_t) |\n"
  "+-------------+---------------+-------------------+\n"
  "| 1           | 10            | def_article       |\n"
  "| 2           | 10            | noun              |\n"
  "| 3           | 11            | relative_pronoun  |\n"
  "| 4           | 12            | verb              |\n"
  "+-------------+---------------+-------------------+\n"
  ""),


//getting features from word object 4
QueryAnswer(false, "getting features from word object 4",
  "GET FEATURES parent, sploinks\n"
  "FROM OBJECTS WITH ID_DS = 1,2,2,2,3,3,4\n"
  "[Word]\n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Feature 'sploinks' does not exist for the given object type.\n"
  ""),


//getting features from word object 4.5
QueryAnswer(false, "getting features from word object 4.5",
  "GET FEATURES parent,lemma\n"
  "FROM OBJECTS WITH ID_DS = 1,2,2,2,3,3,4\n"
  "[Word]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+---------------+----------------+\n"
  "| id_d : id_d | parent : id_d | lemma : string |\n"
  "+-------------+---------------+----------------+\n"
  "| 1           | 10            | the            |\n"
  "| 2           | 10            | door           |\n"
  "| 3           | 11            | which          |\n"
  "| 4           | 12            | open           |\n"
  "+-------------+---------------+----------------+\n"
  ""),


//getting features from word object 5
QueryAnswer(false, "getting features from word object 5",
  "GET FEATURES parent, surface\n"
  "FROM OBJECTS WITH ID_DS = 1,2,2,2,3,3,4\n"
  "[Sploinks]\n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Object type name 'Sploinks' does not exist.\n"
  ""),


//getting string set from word.surface
QueryAnswer(false, "getting string set from word.surface",
  "GET SET FROM FEATURE surface\n"
  "[word]\n"
  "GO\n"
  "",
  true, true,
  "+----------------+\n"
  "| value : string |\n"
  "+----------------+\n"
  "| The            |\n"
  "| door,          |\n"
  "| which          |\n"
  "| opened         |\n"
  "| towards        |\n"
  "| the            |\n"
  "| East,          |\n"
  "| was            |\n"
  "| blue.          |\n"
  "+----------------+\n"
  ""),


//getting string set from word.lemma
QueryAnswer(false, "getting string set from word.lemma",
  "GET SET FROM FEATURE lemma\n"
  "[word]\n"
  "GO\n"
  "",
  true, true,
  "+----------------+\n"
  "| value : string |\n"
  "+----------------+\n"
  "| the            |\n"
  "| door           |\n"
  "| which          |\n"
  "| open           |\n"
  "| towards        |\n"
  "| east           |\n"
  "| be             |\n"
  "| blue           |\n"
  "+----------------+\n"
  ""),


//selecting all objects having monads in
QueryAnswer(false, "selecting all objects having monads in",
  "SELECT OBJECTS HAVING MONADS IN {1-2, 6-7}\n"
  "[ALL]\n"
  "GO\n"
  "",
  true, true,
  "+---------------------------+-------------------+-------------+\n"
  "| object_type_name : string | monad_m : monad_m | id_d : id_d |\n"
  "+---------------------------+-------------------+-------------+\n"
  "| word                      | 1                 | 1           |\n"
  "| word                      | 2                 | 2           |\n"
  "| word                      | 6                 | 6           |\n"
  "| word                      | 7                 | 7           |\n"
  "| phrase                    | 1                 | 10          |\n"
  "| phrase                    | 6                 | 13          |\n"
  "| phrase                    | 6                 | 14          |\n"
  "| clause_atom               | 1                 | 17          |\n"
  "| clause_atom               | 6                 | 18          |\n"
  "| clause                    | 1                 | 20          |\n"
  "| clause                    | 6                 | 21          |\n"
  "| sentence                  | 1                 | 22          |\n"
  "| paragraph                 | 1                 | 23          |\n"
  "+---------------------------+-------------------+-------------+\n"
  ""),


//selecting word objects having monads in
QueryAnswer(false, "selecting word objects having monads in",
  "SELECT OBJECTS HAVING MONADS IN {1-2, 6-7}\n"
  "[word]\n"
  "GO\n"
  "",
  true, true,
  "+---------------------------+-------------------+-------------+\n"
  "| object_type_name : string | monad_m : monad_m | id_d : id_d |\n"
  "+---------------------------+-------------------+-------------+\n"
  "| word                      | 1                 | 1           |\n"
  "| word                      | 2                 | 2           |\n"
  "| word                      | 6                 | 6           |\n"
  "| word                      | 7                 | 7           |\n"
  "+---------------------------+-------------------+-------------+\n"
  ""),


//selecting phrase objects having monads in
QueryAnswer(false, "selecting phrase objects having monads in",
  "SELECT OBJECTS HAVING MONADS IN {1-2, 6-7}\n"
  "[phrase]\n"
  "GO\n"
  "",
  true, true,
  "+---------------------------+-------------------+-------------+\n"
  "| object_type_name : string | monad_m : monad_m | id_d : id_d |\n"
  "+---------------------------+-------------------+-------------+\n"
  "| phrase                    | 1                 | 10          |\n"
  "| phrase                    | 6                 | 13          |\n"
  "| phrase                    | 6                 | 14          |\n"
  "+---------------------------+-------------------+-------------+\n"
  ""),


//selecting clause_atom objects having monads in
QueryAnswer(false, "selecting clause_atom objects having monads in",
  "SELECT OBJECTS HAVING MONADS IN {1-2, 6-7}\n"
  "[clause_atom]\n"
  "GO\n"
  "",
  true, true,
  "+---------------------------+-------------------+-------------+\n"
  "| object_type_name : string | monad_m : monad_m | id_d : id_d |\n"
  "+---------------------------+-------------------+-------------+\n"
  "| clause_atom               | 1                 | 17          |\n"
  "| clause_atom               | 6                 | 18          |\n"
  "+---------------------------+-------------------+-------------+\n"
  ""),


//selecting clause objects having monads in
QueryAnswer(false, "selecting clause objects having monads in",
  "SELECT OBJECTS HAVING MONADS IN {1-2, 6-7}\n"
  "[clause]\n"
  "GO\n"
  "",
  true, true,
  "+---------------------------+-------------------+-------------+\n"
  "| object_type_name : string | monad_m : monad_m | id_d : id_d |\n"
  "+---------------------------+-------------------+-------------+\n"
  "| clause                    | 1                 | 20          |\n"
  "| clause                    | 6                 | 21          |\n"
  "+---------------------------+-------------------+-------------+\n"
  ""),


//selecting sentence objects having monads in
QueryAnswer(false, "selecting sentence objects having monads in",
  "SELECT OBJECTS HAVING MONADS IN {1-2, 6-7}\n"
  "[sentence]\n"
  "GO\n"
  "",
  true, true,
  "+---------------------------+-------------------+-------------+\n"
  "| object_type_name : string | monad_m : monad_m | id_d : id_d |\n"
  "+---------------------------+-------------------+-------------+\n"
  "| sentence                  | 1                 | 22          |\n"
  "+---------------------------+-------------------+-------------+\n"
  ""),


//selecting paragraph objects having monads in
QueryAnswer(false, "selecting paragraph objects having monads in",
  "SELECT OBJECTS HAVING MONADS IN {1-2, 6-7}\n"
  "[paragraph]\n"
  "GO\n"
  "",
  true, true,
  "+---------------------------+-------------------+-------------+\n"
  "| object_type_name : string | monad_m : monad_m | id_d : id_d |\n"
  "+---------------------------+-------------------+-------------+\n"
  "| paragraph                 | 1                 | 23          |\n"
  "+---------------------------+-------------------+-------------+\n"
  ""),


//selecting word objects at
QueryAnswer(false, "selecting word objects at",
  "SELECT OBJECTS AT MONAD = 7\n"
  "[word]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 7           |\n"
  "+-------------+\n"
  ""),


//selecting phrase objects at 1
QueryAnswer(false, "selecting phrase objects at 1",
  "SELECT OBJECTS AT MONAD = 7\n"
  "[phrase]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "+-------------+\n"
  ""),


//selecting phrase objects at 2
QueryAnswer(false, "selecting phrase objects at 2",
  "SELECT OBJECTS AT MONAD = 1\n"
  "[phrase]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 10          |\n"
  "+-------------+\n"
  ""),


//selecting clause objects at
QueryAnswer(false, "selecting clause objects at",
  "SELECT OBJECTS AT MONAD = 1\n"
  "[clause]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 20          |\n"
  "+-------------+\n"
  ""),


//selecting clause_atom objects at
QueryAnswer(false, "selecting clause_atom objects at",
  "SELECT OBJECTS AT MONAD = 1\n"
  "[clause_atom]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 17          |\n"
  "+-------------+\n"
  ""),


//selecting sentence objects at
QueryAnswer(false, "selecting sentence objects at",
  "SELECT OBJECTS AT MONAD = 1\n"
  "[sentence]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 22          |\n"
  "+-------------+\n"
  ""),


//selecting paragraph objects at
QueryAnswer(false, "selecting paragraph objects at",
  "SELECT OBJECTS AT MONAD = 1\n"
  "[paragraph]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 23          |\n"
  "+-------------+\n"
  ""),


//selecting objects having monads in: all
QueryAnswer(false, "selecting objects having monads in: all",
  "SELECT OBJECTS HAVING MONADS IN {1-2, 4-7}\n"
  "[ALL]\n"
  "GO\n"
  "",
  true, true,
  "+---------------------------+-------------------+-------------+\n"
  "| object_type_name : string | monad_m : monad_m | id_d : id_d |\n"
  "+---------------------------+-------------------+-------------+\n"
  "| word                      | 1                 | 1           |\n"
  "| word                      | 2                 | 2           |\n"
  "| word                      | 4                 | 4           |\n"
  "| word                      | 5                 | 5           |\n"
  "| word                      | 6                 | 6           |\n"
  "| word                      | 7                 | 7           |\n"
  "| phrase                    | 1                 | 10          |\n"
  "| phrase                    | 4                 | 12          |\n"
  "| phrase                    | 5                 | 13          |\n"
  "| phrase                    | 6                 | 14          |\n"
  "| clause_atom               | 1                 | 17          |\n"
  "| clause_atom               | 4                 | 18          |\n"
  "| clause                    | 1                 | 20          |\n"
  "| clause                    | 4                 | 21          |\n"
  "| sentence                  | 1                 | 22          |\n"
  "| paragraph                 | 1                 | 23          |\n"
  "+---------------------------+-------------------+-------------+\n"
  ""),


//topographic query on realistic db number 1
QueryAnswer(false, "topographic query on realistic db number 1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 1.1
QueryAnswer(false, "topographic query on realistic db number 1.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [word]*\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 1.2
QueryAnswer(false, "topographic query on realistic db number 1.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [word pos=def_article]\n"
  "  [word]*{1,3}\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 1.3
QueryAnswer(false, "topographic query on realistic db number 1.3",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [word pos=def_article]\n"
  "  [word]*{0-1}\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 2
QueryAnswer(false, "topographic query on realistic db number 2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Sentence\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ sentence 22 { 1-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 2.1
QueryAnswer(false, "topographic query on realistic db number 2.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [clause_atom]\n"
  "   [clause_atom]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  "[ clause_atom 19 { 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 2.1.1
QueryAnswer(false, "topographic query on realistic db number 2.1.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [clause_atom]\n"
  "   [gap? retrieve]\n"
  "   [clause_atom]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  "[ pow_m { 3-7 } false  //  <  > \n"
  " ]\n"
  "[ clause_atom 19 { 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 2.1.2
QueryAnswer(false, "topographic query on realistic db number 2.1.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [clause_atom]\n"
  "   [gap focus]\n"
  "   [clause_atom]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  "[ pow_m { 3-7 } true  //  <  > \n"
  " ]\n"
  "[ clause_atom 19 { 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 2.1.3
QueryAnswer(false, "topographic query on realistic db number 2.1.3",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [clause_atom]\n"
  "   [gap? focus]\n"
  "   [clause_atom]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  "[ pow_m { 3-7 } true  //  <  > \n"
  " ]\n"
  "[ clause_atom 19 { 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 2.2
QueryAnswer(false, "topographic query on realistic db number 2.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Sentence\n"
  "  [Clause\n"
  "     [gap? retrieve\n"
  "        [clause_atom]\n"
  "     ]\n"
  "  ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ sentence 22 { 1-9 } false  //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ pow_m { 3-7 } false  //  <  < [ clause_atom 18 { 3-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 3
QueryAnswer(false, "topographic query on realistic db number 3",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 3.1
QueryAnswer(false, "topographic query on realistic db number 3.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [gap\n"
  "      [phrase first phrase_function HAS Rela]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 3.1.1
QueryAnswer(false, "topographic query on realistic db number 3.1.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [gap\n"
  "      [phrase first phrase_function = Rela]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The list feature phrase_function is being compared with an enumeration constant, but the comparison operator is not HAS, as it should be.\n"
  ""),


//topographic query on realistic db number 3.1.2
QueryAnswer(false, "topographic query on realistic db number 3.1.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [gap retrieve\n"
  "      [phrase first phrase_function HAS Rela]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ pow_m { 3-7 } false  //  <  < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 3.2
QueryAnswer(false, "topographic query on realistic db number 3.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [gap\n"
  "      [phrase phrase_type=NP]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 3.3
QueryAnswer(false, "topographic query on realistic db number 3.3",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause_atom\n"
  "   [gap]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 3.4
QueryAnswer(false, "topographic query on realistic db number 3.4",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause_atom\n"
  "   [gap?]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause_atom 18 { 3-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause_atom 19 { 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 3.5
QueryAnswer(false, "topographic query on realistic db number 3.5",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [word lemma = \"DoesNotExist\"]\n"
  "   [word lemma = \"AlsoDoesNotExist\"]\n"
  "   OR\n"
  "   [Word lemma = \"CertainlyDoesNotExist\"]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 3.6
QueryAnswer(false, "topographic query on realistic db number 3.6",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [word lemma = \"DoesNotExist\"]\n"
  "   [word lemma = \"AlsoDoesNotExist\"]\n"
  "   OR\n"
  "   [Word lemma = \"door\"]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false  //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 3.7
QueryAnswer(false, "topographic query on realistic db number 3.7",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [gap?]\n"
  "   [Word lemma = \"DoesNotExist\"]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 3.8
QueryAnswer(false, "topographic query on realistic db number 3.8",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [gap]\n"
  "   [Word lemma = \"DoesNotExist\"]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 3.9
QueryAnswer(false, "topographic query on realistic db number 3.9",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [gap\n"
  "     [Word lemma = \"DoesNotExist\"]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 3.10
QueryAnswer(false, "topographic query on realistic db number 3.10",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "  [word first and last\n"
  "     lemma=\"be\"\n"
  "  ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 15 { 8 } false  //  <  < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 4
QueryAnswer(false, "topographic query on realistic db number 4",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause_atom\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause_atom 18 { 3-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause_atom 19 { 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 5
QueryAnswer(false, "topographic query on realistic db number 5",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 6
QueryAnswer(false, "topographic query on realistic db number 6",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 7
QueryAnswer(false, "topographic query on realistic db number 7",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word first\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 7.1
QueryAnswer(false, "topographic query on realistic db number 7.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word first]\n"
  ".. < 3\n"
  "[word]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 7.2
QueryAnswer(false, "topographic query on realistic db number 7.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word first]\n"
  ".. <= 3\n"
  "[word]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 7.3
QueryAnswer(false, "topographic query on realistic db number 7.3",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word first]\n"
  ".. BETWEEN 2 AND 3\n"
  "[word]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 7.4
QueryAnswer(false, "topographic query on realistic db number 7.4",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word first]\n"
  ".. BETWEEN 2 AND 120\n"
  "[word]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 8
QueryAnswer(false, "topographic query on realistic db number 8",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word last\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 9
QueryAnswer(false, "topographic query on realistic db number 9",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase as p1\n"
  "  [word parent = p1.self]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false  //  <  < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false  //  <  < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 8 } false  //  <  < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 9 } false  //  <  < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 9.1
QueryAnswer(false, "topographic query on realistic db number 9.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[word as w1\n"
  "   [Phrase self = w1.parent]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 3 { 3 } false  //  <  < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  < [ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  < [ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false  //  <  < [ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 10
QueryAnswer(false, "topographic query on realistic db number 10",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase as p1\n"
  "   [Phrase parent = p1.self]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Feature parent does not exist on its enclosing object type.\n"
  ""),


//topographic query on realistic db number 11
QueryAnswer(false, "topographic query on realistic db number 11",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase first GET phrase_function, phrase_type\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false (phrase_function=\" Subj \",phrase_type=\"NP\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 11.1
QueryAnswer(false, "topographic query on realistic db number 11.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word first GET surface, pos, lemma, parent\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false (surface=\"The\",pos=\"def_article\",lemma=\"the\",parent=\"10\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 11.2
QueryAnswer(false, "topographic query on realistic db number 11.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase last GET self, phrase_type\n"
  "  [Word first GET self]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 16 { 9 } false (self=\"16\",phrase_type=\"AP\") //  <  < [ word 9 { 9 } false (self=\"9\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 12
QueryAnswer(false, "topographic query on realistic db number 12",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [Clause\n"
  "     [Clause_atom\n"
  "        [Phrase\n"
  "           [Word]\n"
  "        ]\n"
  "     ]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ clause_atom 17 { 1-2 } false  //  <  < [ phrase 10 { 1-2 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause_atom 19 { 8-9 } false  //  <  < [ phrase 15 { 8 } false  //  <  < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 9 } false  //  <  < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  < [ clause_atom 18 { 3-7 } false  //  <  < [ phrase 11 { 3 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false  //  <  < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false  //  <  < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 12.1
QueryAnswer(false, "topographic query on realistic db number 12.1",
  "SELECT ALL OBJECTS\n"
  "RETURNING FLAT SHEAF\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [Clause\n"
  "     [Clause_atom\n"
  "        [Phrase\n"
  "           [Word]\n"
  "        ]\n"
  "     ]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " // <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 10 { 1-2 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  "[ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  "[ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  "[ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  "[ clause_atom 19 { 8-9 } false  //  <  > \n"
  " ]\n"
  "[ clause_atom 18 { 3-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause 20 { 1-2, 8-9 } false  //  <  > \n"
  " ]\n"
  "[ clause 21 { 3-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ paragraph 23 { 1-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//topographic query on realistic db number 13
QueryAnswer(false, "topographic query on realistic db number 13",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[phrase phrase_type IN (Subj,Attr)]\n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant Subj does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 13.1
QueryAnswer(false, "topographic query on realistic db number 13.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[phrase phrase_type IN (VP) OR phrase_type IN (PP,AP)]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 13.2
QueryAnswer(false, "topographic query on realistic db number 13.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[phrase phrase_type IN (NP,AP)]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 13.3
QueryAnswer(false, "topographic query on realistic db number 13.3",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[phrase phrase_type IN (NP) AND phrase_function HAS Subj]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 13.4
QueryAnswer(false, "topographic query on realistic db number 13.4",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[phrase NOT (NOT phrase_type IN (NP) OR NOT phrase_function HAS Subj)]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 13.5
QueryAnswer(false, "topographic query on realistic db number 13.5",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[phrase NOT phrase_type IN (AP,NP,PP)]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 13.6
QueryAnswer(false, "topographic query on realistic db number 13.6",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[phrase NOT (phrase_type IN (VP) AND (phrase_function HAS Pred OR phrase_function HAS PreC))]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 14
QueryAnswer(false, "topographic query on realistic db number 14",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[word surface ~ '^[Tt]']\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 14.1
QueryAnswer(false, "topographic query on realistic db number 14.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[word surface ~ 'h$']\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 14.2
QueryAnswer(false, "topographic query on realistic db number 14.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[word lemma ~ 'e$']\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 14.3
QueryAnswer(false, "topographic query on realistic db number 14.3",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[word lemma !~ 'e$']\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 14.4
QueryAnswer(false, "topographic query on realistic db number 14.4",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[word NOT lemma !~ 'e$']\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 14.5
QueryAnswer(false, "topographic query on realistic db number 14.5",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[word NOT NOT NOT lemma !~ 'e$']\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 14.6
QueryAnswer(false, "topographic query on realistic db number 14.6",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[word NOT NOT NOT pos !~ 'e$']\n"
  "GO\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature pos is being compared with a string.  The feature is not a string.\n"
  ""),


//topographic query on realistic db number 15
QueryAnswer(false, "topographic query on realistic db number 15",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "GO\n"
  "",
  true, false,
  "Parsing failed\n"
  "syntax error near the token go\n"
  ""),


//topographic query on realistic db number 15.1
QueryAnswer(false, "topographic query on realistic db number 15.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "  [Word]\n"
  "  [gap focus\n"
  "     [Phrase first]\n"
  "  ]\n"
  "  [Phrase]\n"
  "  [Phrase last]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ pow_m { 3-7 } true  //  <  < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.2
QueryAnswer(false, "topographic query on realistic db number 15.2",
  "SELECT ALL OBJECTS\n"
  "IN {1-2, 8-9}\n"
  "WHERE\n"
  "[Clause_atom]\n"
  "OR\n"
  "[Clause]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause 20 { 1-2, 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause_atom 19 { 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.3
QueryAnswer(false, "topographic query on realistic db number 15.3",
  "SELECT ALL OBJECTS\n"
  "IN {1-2, 7-8}\n"
  "WHERE\n"
  "[Clause_atom]\n"
  "OR\n"
  "[Clause]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.4
QueryAnswer(false, "topographic query on realistic db number 15.4",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "  [Word as p1]\n"
  "]\n"
  "OR\n"
  "[Phrase\n"
  "  [Word surface=p1.surface]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'p1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//topographic query on realistic db number 15.5
QueryAnswer(false, "topographic query on realistic db number 15.5",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "  [Word as p1]*\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "You may not have an object reference on an object block with a * operator.\n"
  ""),


//topographic query on realistic db number 15.6
QueryAnswer(false, "topographic query on realistic db number 15.6",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase \n"
  "  [Word]\n"
  "  [Word first]\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The 'first' modifier must only be used for the first object block in a blocks.\n"
  ""),


//topographic query on realistic db number 15.7
QueryAnswer(false, "topographic query on realistic db number 15.7",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "  [Word last]\n"
  "  [Word]\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The 'last' modifier must only be used for the last object block in a blocks.\n"
  ""),


//topographic query on realistic db number 15.8
QueryAnswer(false, "topographic query on realistic db number 15.8",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "  [Word self = p1.self]\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The object reference has not been declared in the object reference usage 'p1.self'.\n"
  ""),


//topographic query on realistic db number 15.9
QueryAnswer(false, "topographic query on realistic db number 15.9",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase as p1\n"
  "  [Word parent = p1.does_not_exist]\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The feature does not exist on the object type of the object reference\n"
  "in the object reference usage 'p1.does_not_exist'.\n"
  ""),


//topographic query on realistic db number 15.10
QueryAnswer(false, "topographic query on realistic db number 15.10",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type = DoesNotExist]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant DoesNotExist does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.11
QueryAnswer(false, "topographic query on realistic db number 15.11",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type ~ \"\"]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature phrase_type is being compared with a string.  The feature is not a string.\n"
  ""),


//topographic query on realistic db number 15.12
QueryAnswer(false, "topographic query on realistic db number 15.12",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type !~ \"\"]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature phrase_type is being compared with a string.  The feature is not a string.\n"
  ""),


//topographic query on realistic db number 15.13
QueryAnswer(false, "topographic query on realistic db number 15.13",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type !~ blah]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The '~' and '!~' tilde operators (regular expression matching) can only be used with strings on the right-hand side.\n"
  ""),


//topographic query on realistic db number 15.14
QueryAnswer(false, "topographic query on realistic db number 15.14",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type ~ blah]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The '~' and '!~' tilde operators (regular expression matching) can only be used with strings on the right-hand side.\n"
  ""),


//topographic query on realistic db number 15.15
QueryAnswer(false, "topographic query on realistic db number 15.15",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type !~ 1]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The '~' and '!~' tilde operators (regular expression matching) can only be used with strings on the right-hand side.\n"
  ""),


//topographic query on realistic db number 15.16
QueryAnswer(false, "topographic query on realistic db number 15.16",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type ~ 1]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The '~' and '!~' tilde operators (regular expression matching) can only be used with strings on the right-hand side.\n"
  ""),


//topographic query on realistic db number 15.17
QueryAnswer(false, "topographic query on realistic db number 15.17",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type ~ NIL]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The '~' and '!~' tilde operators (regular expression matching) can only be used with strings on the right-hand side.\n"
  ""),


//topographic query on realistic db number 15.18
QueryAnswer(false, "topographic query on realistic db number 15.18",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase p1 phrase_function = p1.self]\n"
  "",
  true, false,
  "Parsing failed\n"
  "syntax error near the string 'phrase_function'\n"
  ""),


//topographic query on realistic db number 15.19
QueryAnswer(false, "topographic query on realistic db number 15.19",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type IN (NP, Subj, DoesNotExist)]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant Subj does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.20
QueryAnswer(false, "topographic query on realistic db number 15.20",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type IN (NP, DoesNotExist)]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant DoesNotExist does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.21
QueryAnswer(false, "topographic query on realistic db number 15.21",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause_atom\n"
  "  [Phrase\n"
  "     [Word as p1]\n"
  "  ]*\n"
  "  [Word surface = p1.surface]\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'p1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//topographic query on realistic db number 15.22
QueryAnswer(false, "topographic query on realistic db number 15.22",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_function = Subj]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The list feature phrase_function is being compared with an enumeration constant, but the comparison operator is not HAS, as it should be.\n"
  ""),


//topographic query on realistic db number 15.23
QueryAnswer(false, "topographic query on realistic db number 15.23",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type HAS Subj]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant Subj does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.24
QueryAnswer(false, "topographic query on realistic db number 15.24",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type < Pred]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant Pred does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.25
QueryAnswer(false, "topographic query on realistic db number 15.25",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_function IN (1,2,3)]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature phrase_function is being compared with a list of integers.  The feature is neither of type INTEGER nor of type ID_D.\n"
  ""),


//topographic query on realistic db number 15.26
QueryAnswer(false, "topographic query on realistic db number 15.26",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_function HAS 1]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The list feature phrase_function is being compared with a integer using the HAS operator, but the list is neither a list of integers, nor a list of id_ds.\n"
  ""),


//topographic query on realistic db number 15.27
QueryAnswer(false, "topographic query on realistic db number 15.27",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type > 1]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature phrase_type is being compared with an integer.  The feature is neither an integer nor an id_d.\n"
  ""),


//topographic query on realistic db number 15.28
QueryAnswer(false, "topographic query on realistic db number 15.28",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type <= \"a\"]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature phrase_type is being compared with a string.  The feature is not a string.\n"
  ""),


//topographic query on realistic db number 15.29
QueryAnswer(false, "topographic query on realistic db number 15.29",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase as p1\n"
  "  [Phrase phrase_function HAS p1.phrase_type]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature phrase_function is being compared\n"
  "using HAS and an object reference usage. The feature is a list of enum\n"
  "constants, but the object reference usage is not the same enum.\n"
  ""),


//topographic query on realistic db number 15.30
QueryAnswer(false, "topographic query on realistic db number 15.30",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as w1\n"
  "  [Phrase phrase_function HAS w1.surface]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature phrase_function is being compared\n"
  "using HAS and an object reference usage. The feature is a list of enum\n"
  "constants, but the object reference usage is not an enum.\n"
  ""),


//topographic query on realistic db number 15.31
QueryAnswer(false, "topographic query on realistic db number 15.31",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase as p1\n"
  "  [Phrase phrase_function HAS p1.phrase_function]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature phrase_function is being compared\n"
  "using HAS and an object reference usage.\n"
  "The feature is a list, but so is the object reference usage.\n"
  "The HAS operator is only for comparing lists with atomic values.\n"
  ""),


//topographic query on realistic db number 15.32
QueryAnswer(false, "topographic query on realistic db number 15.32",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase as p1\n"
  "  [Phrase phrase_type HAS p1.phrase_function]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature phrase_type is being compared\n"
  "using HAS and an object reference usage.\n"
  "The feature is not a list.\n"
  "The HAS operator is only for comparing lists with atomic values.\n"
  ""),


//topographic query on realistic db number 15.33
QueryAnswer(false, "topographic query on realistic db number 15.33",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word surface IN (1,2,3)]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature surface is being compared with a list of integers.  The feature is neither of type INTEGER nor of type ID_D.\n"
  ""),


//topographic query on realistic db number 15.34
QueryAnswer(false, "topographic query on realistic db number 15.34",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[DoesNotExist]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Object type name 'doesnotexist' does not exist.\n"
  ""),


//topographic query on realistic db number 15.35
QueryAnswer(false, "topographic query on realistic db number 15.35",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "NOTEXIST [Word]*\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "You may not have a NOTEXIST on an object block with a * operator.\n"
  ""),


//topographic query on realistic db number 15.36
QueryAnswer(false, "topographic query on realistic db number 15.36",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1]*\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "You may not have an object reference on an object block with a * operator.\n"
  ""),


//topographic query on realistic db number 15.37
QueryAnswer(false, "topographic query on realistic db number 15.37",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[word]\n"
  "[word first and last]\n"
  "[word]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The 'first and last' modifier must only be used object blocks that are both first and last in their blocks.\n"
  ""),


//topographic query on realistic db number 15.38
QueryAnswer(false, "topographic query on realistic db number 15.38",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[phrase as p1]\n"
  "[phrase as p1]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Object reference p1 has already been declared once.  An object reference must not be declared more than once.\n"
  ""),


//topographic query on realistic db number 15.39
QueryAnswer(false, "topographic query on realistic db number 15.39",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word]\n"
  ".. < -1\n"
  "[Word]\n"
  "",
  true, false,
  "Parsing failed\n"
  "syntax error near the token -\n"
  ""),


//topographic query on realistic db number 15.40
QueryAnswer(false, "topographic query on realistic db number 15.40",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word]\n"
  ".. <= -1\n"
  "[Word]\n"
  "",
  true, false,
  "Parsing failed\n"
  "syntax error near the token -\n"
  ""),


//topographic query on realistic db number 15.41
QueryAnswer(false, "topographic query on realistic db number 15.41",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word]\n"
  ".. BETWEEN 5 and 3\n"
  "[Word]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: Something wrong with a restrictor on a power block (\"..\").\n"
  "If the restrictor is \"< integer\", the integer must not be 0.\n"
  "If the restrictor is \"between X and Y\", then X must be less than or\n"
  "equal to Y.\n"
  ""),


//topographic query on realistic db number 15.42
QueryAnswer(false, "topographic query on realistic db number 15.42",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [gap]\n"
  "   [gap]\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: You cannot have two gap_blocks or opt_gap_blocks next to each other,\n"
  "       nor can you have an opt_gap_block next to a gap_block.\n"
  ""),


//topographic query on realistic db number 15.43
QueryAnswer(false, "topographic query on realistic db number 15.43",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [gap?]\n"
  "   [gap?]\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: You cannot have two gap_blocks or opt_gap_blocks next to each other,\n"
  "       nor can you have an opt_gap_block next to a gap_block.\n"
  ""),


//topographic query on realistic db number 15.44
QueryAnswer(false, "topographic query on realistic db number 15.44",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [gap?]\n"
  "   [gap]\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: You cannot have two gap_blocks or opt_gap_blocks next to each other,\n"
  "       nor can you have an opt_gap_block next to a gap_block.\n"
  ""),


//topographic query on realistic db number 15.45
QueryAnswer(false, "topographic query on realistic db number 15.45",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [gap]\n"
  "   [gap?]\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: You cannot have two gap_blocks or opt_gap_blocks next to each other,\n"
  "       nor can you have an opt_gap_block next to a gap_block.\n"
  ""),


//topographic query on realistic db number 15.46
QueryAnswer(false, "topographic query on realistic db number 15.46",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [Word]\n"
  "   ..\n"
  "   [gap]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.47
QueryAnswer(false, "topographic query on realistic db number 15.47",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [Word]\n"
  "   ..\n"
  "   [gap?]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false  //  <  < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.48
QueryAnswer(false, "topographic query on realistic db number 15.48",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [Word]\n"
  "   ..\n"
  "   NOTEXIST [Word]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.49
QueryAnswer(false, "topographic query on realistic db number 15.49",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [Word]\n"
  "   ..\n"
  "   ..\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: You cannot have two power blocks (i.e., \"..\") next to each other.\n"
  ""),


//topographic query on realistic db number 15.50
QueryAnswer(false, "topographic query on realistic db number 15.50",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Gap\n"
  "      [DoesNotExist]\n"
  "   ]\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Object type name 'doesnotexist' does not exist.\n"
  ""),


//topographic query on realistic db number 15.51
QueryAnswer(false, "topographic query on realistic db number 15.51",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word as p1]\n"
  "   OR\n"
  "   [Gap\n"
  "      [Word lemma = p1.self]\n"
  "   ]\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'p1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//topographic query on realistic db number 15.52
QueryAnswer(false, "topographic query on realistic db number 15.52",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Gap\n"
  "      [Word lemma = 1]\n"
  "   ]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature lemma is being compared with an integer.  The feature is neither an integer nor an id_d.\n"
  ""),


//topographic query on realistic db number 15.53
QueryAnswer(false, "topographic query on realistic db number 15.53",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [gap?\n"
  "      [DoesNotExist]\n"
  "   ]\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Object type name 'doesnotexist' does not exist.\n"
  ""),


//topographic query on realistic db number 15.54
QueryAnswer(false, "topographic query on realistic db number 15.54",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   NOTEXIST [DoesNotExist]\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Object type name 'doesnotexist' does not exist.\n"
  ""),


//topographic query on realistic db number 15.55
QueryAnswer(false, "topographic query on realistic db number 15.55",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word as p1]\n"
  "   OR\n"
  "   [Gap?\n"
  "      [Word lemma = p1.self]\n"
  "   ]\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'p1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//topographic query on realistic db number 15.56
QueryAnswer(false, "topographic query on realistic db number 15.56",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word as p1]\n"
  "   OR\n"
  "   NOTEXIST [Word\n"
  "      [Word lemma = p1.self]\n"
  "   ]\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'p1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//topographic query on realistic db number 15.57
QueryAnswer(false, "topographic query on realistic db number 15.57",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word as p1]\n"
  "   OR\n"
  "   [Word\n"
  "      [Word lemma = p1.self]\n"
  "   ]*\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'p1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//topographic query on realistic db number 15.58
QueryAnswer(false, "topographic query on realistic db number 15.58",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Gap?\n"
  "      [Word lemma = 1]\n"
  "   ]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature lemma is being compared with an integer.  The feature is neither an integer nor an id_d.\n"
  ""),


//topographic query on realistic db number 15.59
QueryAnswer(false, "topographic query on realistic db number 15.59",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase as p1]\n"
  "OR\n"
  "[Phrase phrase_function HAS p1.phrase_function]\n"
  "OR\n"
  "[Phrase]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'p1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//topographic query on realistic db number 15.60
QueryAnswer(false, "topographic query on realistic db number 15.60",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase DoesNotExist = 1]\n"
  "OR\n"
  "[Phrase]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Feature DoesNotExist does not exist on its enclosing object type.\n"
  ""),


//topographic query on realistic db number 15.61
QueryAnswer(false, "topographic query on realistic db number 15.61",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_function HAS 1]\n"
  "OR\n"
  "[Phrase]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The list feature phrase_function is being compared with a integer using the HAS operator, but the list is neither a list of integers, nor a list of id_ds.\n"
  ""),


//topographic query on realistic db number 15.62
QueryAnswer(false, "topographic query on realistic db number 15.62",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word surface ~'[Th']\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error compiling regular expression.  PCRE2 reported:\n"
  "missing terminating ] for character class\n"
  "Error occured at characterposition 3 in the pattern.\n"
  ""),


//topographic query on realistic db number 15.63
QueryAnswer(false, "topographic query on realistic db number 15.63",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[DoesNotExist]*\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Object type name 'doesnotexist' does not exist.\n"
  ""),


//topographic query on realistic db number 15.64
QueryAnswer(false, "topographic query on realistic db number 15.64",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word\n"
  "  NOT lemma <> \"be\"\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.65
QueryAnswer(false, "topographic query on realistic db number 15.65",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word\n"
  "  lemma IN (Bah, Bah, Black, Sheep)\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The feature 'lemma' is not an enumeration, yet the left-hand-side is a list of enumeration constants.\n"
  ""),


//topographic query on realistic db number 15.66
QueryAnswer(false, "topographic query on realistic db number 15.66",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   phrase_function = 1\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The list feature phrase_function is being compared with an integer, but the comparison operator is not HAS, as it should be.\n"
  ""),


//topographic query on realistic db number 15.67
QueryAnswer(false, "topographic query on realistic db number 15.67",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[word parents HAS Sheep\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant Sheep does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.68
QueryAnswer(false, "topographic query on realistic db number 15.68",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   phrase_function HAS Sheep\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant Sheep does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.69
QueryAnswer(false, "topographic query on realistic db number 15.69",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word\n"
  "  NOT magic <> 11\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.70
QueryAnswer(false, "topographic query on realistic db number 15.70",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word\n"
  "  parents HAS def_article\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant def_article does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.71
QueryAnswer(false, "topographic query on realistic db number 15.71",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   phrase_function HAS def_article\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant def_article does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.72
QueryAnswer(false, "topographic query on realistic db number 15.72",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Word parents HAS p1.lemma]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature parents is being compared\n"
  "using HAS and an object reference usage. The feature is a list of id_d, but\n"
  "the object reference usage is not an id_d.\n"
  ""),


//topographic query on realistic db number 15.73
QueryAnswer(false, "topographic query on realistic db number 15.73",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Word magic IN p1.parents]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature magic is being compared\n"
  "using IN and an object reference usage. The feature is an integer, but\n"
  "the object reference usage is not a list of integer.\n"
  ""),


//topographic query on realistic db number 15.74
QueryAnswer(false, "topographic query on realistic db number 15.74",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Word self in p1.parents]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.75
QueryAnswer(false, "topographic query on realistic db number 15.75",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Phrase phrase_type IN p1.parents]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature phrase_type is being compared\n"
  "using IN and an object reference usage. The feature is an enum,\n"
  "but the object reference usage is not a list of enum.\n"
  ""),


//topographic query on realistic db number 15.76
QueryAnswer(false, "topographic query on realistic db number 15.76",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Phrase self in p1.parents]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ word 3 { 3 } false  //  <  < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  < [ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  < [ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false  //  <  < [ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.77
QueryAnswer(false, "topographic query on realistic db number 15.77",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Word self in p1.lemma]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature self is being compared\n"
  "using IN and an object reference usage.\n"
  "The object reference usage is not a list.\n"
  "The IN operator is only for comparing atomic values with lists.\n"
  ""),


//topographic query on realistic db number 15.78
QueryAnswer(false, "topographic query on realistic db number 15.78",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Word parents IN p1.parents]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature parents is being compared\n"
  "using IN and an object reference usage.\n"
  "The object reference usage is a list, but so is the feature.\n"
  "The IN operator is only for comparing atomic values with lists.\n"
  ""),


//topographic query on realistic db number 15.79
QueryAnswer(false, "topographic query on realistic db number 15.79",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Word parents IN p1.lemma]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature parents is being compared\n"
  "using IN and an object reference usage.\n"
  "The object reference usage is not a list.\n"
  "The IN operator is only for comparing atomic values with lists.\n"
  ""),


//topographic query on realistic db number 15.80
QueryAnswer(false, "topographic query on realistic db number 15.80",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase as p1\n"
  "    [Word self in p1.phrase_function]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature self is being compared\n"
  "using IN and an object reference usage. The feature is an id_d, but\n"
  "the object reference usage is not a list of id_d.\n"
  ""),


//topographic query on realistic db number 15.81
QueryAnswer(false, "topographic query on realistic db number 15.81",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Word self = p1.parents]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature self is being compared with an object reference usage whose feature is of a different type.\n"
  ""),


//topographic query on realistic db number 15.82
QueryAnswer(false, "topographic query on realistic db number 15.82",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Word parents <> p1.parents]\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature parents is being compared with an object reference usage. Both are lists, so you must use either IN, HAS, or = as the comparison operator.\n"
  ""),


//topographic query on realistic db number 15.83
QueryAnswer(false, "topographic query on realistic db number 15.83",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "    [Word parents = p1.parents]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false  //  <  < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.84
QueryAnswer(false, "topographic query on realistic db number 15.84",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word \n"
  "   self = \"not_an_id\" AND self = NIL\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature self is being compared with a string.  The feature is not a string.\n"
  ""),


//topographic query on realistic db number 15.85
QueryAnswer(false, "topographic query on realistic db number 15.85",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word self = NIL AND self = p1.self]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.86
QueryAnswer(false, "topographic query on realistic db number 15.86",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word \n"
  "   self = \"not_an_id\" OR self = NIL\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "The feature self is being compared with a string.  The feature is not a string.\n"
  ""),


//topographic query on realistic db number 15.87
QueryAnswer(false, "topographic query on realistic db number 15.87",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word \n"
  "   surface ~ '[Th' AND self = NIL\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error compiling regular expression.  PCRE2 reported:\n"
  "missing terminating ] for character class\n"
  "Error occured at characterposition 3 in the pattern.\n"
  ""),


//topographic query on realistic db number 15.88
QueryAnswer(false, "topographic query on realistic db number 15.88",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word \n"
  "   surface ~ '[Th' OR self = NIL\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error compiling regular expression.  PCRE2 reported:\n"
  "missing terminating ] for character class\n"
  "Error occured at characterposition 3 in the pattern.\n"
  ""),


//topographic query on realistic db number 15.89
QueryAnswer(false, "topographic query on realistic db number 15.89",
  "SELECT ALL OBJECTS\n"
  "IN {1}\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word NOT (self = p1.self AND self = NIL)]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.90
QueryAnswer(false, "topographic query on realistic db number 15.90",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word NOT (self = p1.self OR self = NIL)]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.91
QueryAnswer(false, "topographic query on realistic db number 15.91",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word self = p1.self AND self = NIL]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.92
QueryAnswer(false, "topographic query on realistic db number 15.92",
  "SELECT ALL OBJECTS\n"
  "IN { 3 }\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word self = p1.self OR self = NIL]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ word 3 { 3 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.93
QueryAnswer(false, "topographic query on realistic db number 15.93",
  "SELECT ALL OBJECTS\n"
  "IN {3}\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word self = NIL OR self = p1.self]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ word 3 { 3 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.94
QueryAnswer(false, "topographic query on realistic db number 15.94",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[phrase phrase_type IN (Subj,Attr) AND self = NIL]\n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant Subj does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.95
QueryAnswer(false, "topographic query on realistic db number 15.95",
  "SELECT ALL OBJECTS\n"
  "IN {3}\n"
  "WHERE\n"
  "[Word as p1\n"
  "]\n"
  "OR\n"
  "[Word\n"
  "   self = p1.self OR self = NIL\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'p1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//topographic query on realistic db number 15.96
QueryAnswer(false, "topographic query on realistic db number 15.96",
  "SELECT ALL OBJECTS\n"
  "IN {3}\n"
  "WHERE\n"
  "[Word as p1\n"
  "]\n"
  "OR\n"
  "[Word\n"
  "   self = p1.self AND self = NIL\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'p1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//topographic query on realistic db number 15.97
QueryAnswer(false, "topographic query on realistic db number 15.97",
  "SELECT ALL OBJECTS\n"
  "IN {3}\n"
  "WHERE\n"
  "[Word as p1\n"
  "]\n"
  "OR\n"
  "[Word\n"
  "   self = p1.self AND self = NIL\n"
  "]\n"
  "[Word]\n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'p1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//topographic query on realistic db number 15.98
QueryAnswer(false, "topographic query on realistic db number 15.98",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word NOT self <> p1.self AND self = NIL]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.99
QueryAnswer(false, "topographic query on realistic db number 15.99",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word self = p1.self AND (self = NIL)]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.100
QueryAnswer(false, "topographic query on realistic db number 15.100",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word NOT (self = p1.self AND self <> NIL)]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.101
QueryAnswer(false, "topographic query on realistic db number 15.101",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word NOT ((self = p1.self) AND self <> NIL)]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.102
QueryAnswer(false, "topographic query on realistic db number 15.102",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word NOT ((self = p1.self) AND (self <> NIL AND self <> NIL))]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.103
QueryAnswer(false, "topographic query on realistic db number 15.103",
  "SELECT ALL OBJECTS\n"
  "IN { 9 }\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word self = p1.self AND self = p1.self]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ word 9 { 9 } false  //  <  < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.104
QueryAnswer(false, "topographic query on realistic db number 15.104",
  "SELECT ALL OBJECTS\n"
  "IN { 9 }\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word self = p1.self OR self = p1.self]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ word 9 { 9 } false  //  <  < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 15.105
QueryAnswer(false, "topographic query on realistic db number 15.105",
  "SELECT ALL OBJECTS\n"
  "IN { 9 }\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word NOT (self = p1.self OR (self = p1.self OR self = p1.self))]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.106
QueryAnswer(false, "topographic query on realistic db number 15.106",
  "SELECT ALL OBJECTS\n"
  "IN { 9 }\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word NOT (self <> NIL OR (self = p1.self OR self = p1.self))]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.107
QueryAnswer(false, "topographic query on realistic db number 15.107",
  "SELECT ALL OBJECTS\n"
  "IN { 9 }\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word NOT (self <> NIL OR (self = NIL OR self = p1.self))]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.108
QueryAnswer(false, "topographic query on realistic db number 15.108",
  "SELECT ALL OBJECTS\n"
  "IN { 9 }\n"
  "WHERE\n"
  "[Word as p1\n"
  "   [Word NOT (self <> NIL OR (self = p1.self OR self = NIL))]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 15.109
QueryAnswer(false, "topographic query on realistic db number 15.109",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase phrase_type = DoesNotExist OR self = NIL]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The enumeration constant DoesNotExist does not exist\n"
  "in the enumeration of the feature.\n"
  ""),


//topographic query on realistic db number 15.110
QueryAnswer(false, "topographic query on realistic db number 15.110",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase GET does_not_exist]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Feature 'does_not_exist' does not exist for the given object type.\n"
  ""),


//topographic query on realistic db number 15.111
QueryAnswer(false, "topographic query on realistic db number 15.111",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [word]\n"
  "   [gap?]\n"
  "   [gap?]\n"
  "   [word]\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: You cannot have two gap_blocks or opt_gap_blocks next to each other,\n"
  "       nor can you have an opt_gap_block next to a gap_block.\n"
  ""),


//topographic query on realistic db number 15.112
QueryAnswer(false, "topographic query on realistic db number 15.112",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase GET does_not_exist]\n"
  "[Phrase]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Feature 'does_not_exist' does not exist for the given object type.\n"
  ""),


//topographic query on realistic db number 15.113
QueryAnswer(false, "topographic query on realistic db number 15.113",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "   [Word]\n"
  "   ..\n"
  "   ..\n"
  "   [Word]\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: You cannot have two power blocks (i.e., \"..\") next to each other!\n"
  ""),


//topographic query on realistic db number 15.114
QueryAnswer(false, "topographic query on realistic db number 15.114",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [\n"
  "      ..\n"
  "      [Word]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The first block in a context cannot be a power block (i.e., \"..\").\n"
  ""),


//topographic query on realistic db number 15.115
QueryAnswer(false, "topographic query on realistic db number 15.115",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [\n"
  "      [Word]\n"
  "      ..\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: The power block cannot appear at the beginning or end.\n"
  ""),


//topographic query on realistic db number 15.116
QueryAnswer(false, "topographic query on realistic db number 15.116",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   ..\n"
  "   [\n"
  "      [Word]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The first block in a context cannot be a power block (i.e., \"..\").\n"
  ""),


//topographic query on realistic db number 15.117
QueryAnswer(false, "topographic query on realistic db number 15.117",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [\n"
  "      [Word]\n"
  "   ]\n"
  "   [gap]\n"
  "   ..\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: The power block cannot appear at the beginning or end.\n"
  ""),


//topographic query on realistic db number 15.118
QueryAnswer(false, "topographic query on realistic db number 15.118",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [\n"
  "      [Word]\n"
  "      ..\n"
  "   ]\n"
  "   [gap]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Error: You cannot have a power block (i.e., \"..\") at the end!\n"
  ""),


//topographic query on realistic db number 15.119
QueryAnswer(false, "topographic query on realistic db number 15.119",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]\n"
  "   [\n"
  "      ..\n"
  "      [Word]\n"
  "   ]\n"
  "   [gap]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The first block in a context cannot be a power block (i.e., \"..\").\n"
  ""),


//topographic query on realistic db number 15.120
QueryAnswer(false, "topographic query on realistic db number 15.120",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]\n"
  "   [\n"
  "      [Word]\n"
  "   ]\n"
  "   [gap\n"
  "      ..\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "The first block in a context cannot be a power block (i.e., \"..\").\n"
  ""),


//topographic query on realistic db number 16
QueryAnswer(false, "topographic query on realistic db number 16",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [Phrase\n"
  "    NOTEXIST [Word pos IN (adjective, verb)]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  < [ phrase 10 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 16.1
QueryAnswer(false, "topographic query on realistic db number 16.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "NOTEXIST [Sentence self = 22]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 16.2
QueryAnswer(false, "topographic query on realistic db number 16.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "NOTEXIST [Sentence self = 23]\n"
  "",
  true, true,
  " //  <  > \n"
  ""),


//topographic query on realistic db number 16.3
QueryAnswer(false, "topographic query on realistic db number 16.3",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [Phrase\n"
  "    NOTEXIST [Word pos IN (adjective, verb)]\n"
  "    [Word pos=def_article]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  < [ phrase 10 { 1-2 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false  //  <  < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 16.4
QueryAnswer(false, "topographic query on realistic db number 16.4",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [Phrase\n"
  "    [Word pos=preposition]\n"
  "    NOTEXIST [Word NOT pos IN (def_article,noun)]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  < [ phrase 13 { 5-7 } false  //  <  < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 16.4.1
QueryAnswer(false, "topographic query on realistic db number 16.4.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [Phrase\n"
  "    [Word pos=preposition]\n"
  "    NOTEXIST [Word NOT pos IN (def_article)]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 16.5
QueryAnswer(false, "topographic query on realistic db number 16.5",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [Phrase\n"
  "    [Word pos=preposition]\n"
  "    NOTEXIST [Word pos IN (adjective, verb)]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  < [ phrase 13 { 5-7 } false  //  <  < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 16.6
QueryAnswer(false, "topographic query on realistic db number 16.6",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [Phrase\n"
  "    [Word pos=preposition]\n"
  "    NOTEXIST [Word pos IN (adjective, verb)]\n"
  "    [Word pos=def_article]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  < [ phrase 13 { 5-7 } false  //  <  < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 16.7
QueryAnswer(false, "topographic query on realistic db number 16.7",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  NOTEXIST [Phrase\n"
  "    NOTEXIST [Word pos IN (adjective, verb)]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 16.8
QueryAnswer(false, "topographic query on realistic db number 16.8",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [Phrase phrase_type = NP\n"
  "    NOTEXIST [Phrase\n"
  "      NOTEXIST [Word pos IN (adjective, verb)]\n"
  "    ]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " // \n"
  ""),


//topographic query on realistic db number 16.9
QueryAnswer(false, "topographic query on realistic db number 16.9",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph\n"
  "  [Phrase phrase_type = PP\n"
  "    NOTEXIST [Phrase phrase_type = VP OR phrase_type = AP\n"
  "      NOTEXIST [Word pos IN (adjective, verb)]\n"
  "    ]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false  //  <  < [ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 17
QueryAnswer(false, "topographic query on realistic db number 17",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Paragraph`para\n"
  "  [word`w1`w2 pos=def_article]\n"
  "  [word`w3`w4`w5]*{0-1}\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ paragraph 23 { 1-9 } false `para  //  <  < [ word 1 { 1 } false `w1`w2  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false `w1`w2  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false `w3`w4`w5  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false `w1`w2  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false `w1`w2  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false `w3`w4`w5  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 18
QueryAnswer(false, "topographic query on realistic db number 18",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Sentence`yellow\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ sentence 22 { 1-9 } false `yellow  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 18.1
QueryAnswer(false, "topographic query on realistic db number 18.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause`green\n"
  "   [clause_atom`envious]\n"
  "   [clause_atom`slippery`slope]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false `green  //  <  < [ clause_atom 17 { 1-2 } false `envious  //  <  > \n"
  " ]\n"
  "[ clause_atom 19 { 8-9 } false `slippery`slope  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 18.1.1
QueryAnswer(false, "topographic query on realistic db number 18.1.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause`turquoise\n"
  "   [clause_atom]\n"
  "   [gap?`blue retrieve]\n"
  "   [clause_atom]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false `turquoise  //  <  < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  "[ pow_m { 3-7 } false `blue  //  <  > \n"
  " ]\n"
  "[ clause_atom 19 { 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 18.1.2
QueryAnswer(false, "topographic query on realistic db number 18.1.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [clause_atom]\n"
  "   [gap`my`gaps`are`nice focus]\n"
  "   [clause_atom]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ clause_atom 17 { 1-2 } false  //  <  > \n"
  " ]\n"
  "[ pow_m { 3-7 } true `my`gaps`are`nice  //  <  > \n"
  " ]\n"
  "[ clause_atom 19 { 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 19
QueryAnswer(false, "topographic query on realistic db number 19",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause`flash\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false `flash  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false `flash  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 19.1
QueryAnswer(false, "topographic query on realistic db number 19.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause`Flash\n"
  "   [gap`Gordon retrieve\n"
  "      [phrase`Flash_Gordon first phrase_function HAS Rela]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false `Flash  //  <  < [ pow_m { 3-7 } false `Gordon  //  <  < [ phrase 11 { 3 } false `Flash_Gordon  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 19.2
QueryAnswer(false, "topographic query on realistic db number 19.2",
  "SELECT ALL OBJECTS WHERE\n"
  "[Clause`Flash\n"
  "   [phrase`Rela phrase_function HAS Rela GET phrase_function ]\n"
  "   OR\n"
  "   [phrase`PreC phrase_function HAS PreC GET phrase_function ] \n"
  "   OR\n"
  "   [phrase`PreC phrase_function HAS Attr GET phrase_function ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false `Flash  //  <  < [ phrase 15 { 8 } false `PreC (phrase_function=\" PreC \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 9 } false `PreC (phrase_function=\" Attr \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false `Flash  //  <  < [ phrase 11 { 3 } false `Rela (phrase_function=\" Rela Subj \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 19.3
QueryAnswer(false, "topographic query on realistic db number 19.3",
  "SELECT ALL OBJECTS WHERE\n"
  "[Clause`Flash\n"
  "   [phrase`Rela phrase_function HAS Rela GET phrase_function ]\n"
  "   OR\n"
  "   [phrase`PreC phrase_function HAS PreC GET phrase_function ] \n"
  "   OR\n"
  "   [phrase`PreC phrase_function HAS Attr GET phrase_function ]\n"
  "   OR\n"
  "   [phrase`PreC phrase_function HAS Loca GET phrase_function ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false `Flash  //  <  < [ phrase 15 { 8 } false `PreC (phrase_function=\" PreC \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 9 } false `PreC (phrase_function=\" Attr \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false `Flash  //  <  < [ phrase 11 { 3 } false `Rela (phrase_function=\" Rela Subj \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false `PreC (phrase_function=\" Loca \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.1
QueryAnswer(false, "topographic query on realistic db number 20.1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause`Flash\n"
  "   [\n"
  "      [phrase`Flash_Gordon first phrase_function HAS Rela]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 21 { 3-7 } false `Flash  //  <  < [ phrase 11 { 3 } false `Flash_Gordon  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.2
QueryAnswer(false, "topographic query on realistic db number 20.2",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause`Flash\n"
  "   [Phrase`Rela phrase_function HAS Rela]\n"
  "   [\n"
  "      [phrase`Pred phrase_function HAS Pred]\n"
  "      OR\n"
  "      [phrase`PP phrase_function HAS Loca]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 21 { 3-7 } false `Flash  //  <  < [ phrase 11 { 3 } false `Rela  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false `Pred  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.3
QueryAnswer(false, "topographic query on realistic db number 20.3",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause`Flash\n"
  "   [Phrase`Rela phrase_function HAS Rela]\n"
  "   [\n"
  "      [phrase`Pred phrase_function HAS Pred]\n"
  "      ..\n"
  "      [phrase`PP phrase_function HAS Loca]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 21 { 3-7 } false `Flash  //  <  < [ phrase 11 { 3 } false `Rela  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false `Pred  //  <  > \n"
  " ]\n"
  "[ phrase 13 { 5-7 } false `PP  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.4
QueryAnswer(false, "topographic query on realistic db number 20.4",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause`Flash\n"
  "   [Phrase`Subj phrase_function HAS Subj]\n"
  "   [\n"
  "      [phrase`Rela phrase_function HAS Rela]\n"
  "      OR\n"
  "      [phrase`PreC phrase_function HAS PreC]\n"
  "      OR\n"
  "      [phrase`Attr phrase_function HAS Attr]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false `Flash  //  <  < [ phrase 10 { 1-2 } false `Subj  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false `PreC  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.5
QueryAnswer(false, "topographic query on realistic db number 20.5",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause`Flash\n"
  "   [\n"
  "      [phrase`first GET phrase_function]\n"
  "   ]\n"
  "   [phrase`second GET phrase_function]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false `Flash  //  <  < [ phrase 10 { 1-2 } false `first (phrase_function=\" Subj \") //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false `second (phrase_function=\" PreC \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 8 } false `first (phrase_function=\" PreC \") //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false `second (phrase_function=\" Attr \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false `Flash  //  <  < [ phrase 11 { 3 } false `first (phrase_function=\" Rela Subj \") //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false `second (phrase_function=\" Pred \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false `first (phrase_function=\" Pred \") //  <  > \n"
  " ]\n"
  "[ phrase 13 { 5-7 } false `second (phrase_function=\" Loca \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.6
QueryAnswer(false, "topographic query on realistic db number 20.6",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause`Flash\n"
  "   [\n"
  "      [phrase`first GET phrase_function]\n"
  "   ]\n"
  "   ..\n"
  "   [phrase`second GET phrase_function]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false `Flash  //  <  < [ phrase 10 { 1-2 } false `first (phrase_function=\" Subj \") //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false `second (phrase_function=\" PreC \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 10 { 1-2 } false `first (phrase_function=\" Subj \") //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false `second (phrase_function=\" Attr \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 8 } false `first (phrase_function=\" PreC \") //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false `second (phrase_function=\" Attr \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false `Flash  //  <  < [ phrase 11 { 3 } false `first (phrase_function=\" Rela Subj \") //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false `second (phrase_function=\" Pred \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false `first (phrase_function=\" Rela Subj \") //  <  > \n"
  " ]\n"
  "[ phrase 13 { 5-7 } false `second (phrase_function=\" Loca \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false `first (phrase_function=\" Rela Subj \") //  <  > \n"
  " ]\n"
  "[ phrase 14 { 6-7 } false `second (phrase_function=\" NA \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false `first (phrase_function=\" Pred \") //  <  > \n"
  " ]\n"
  "[ phrase 13 { 5-7 } false `second (phrase_function=\" Loca \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false `first (phrase_function=\" Pred \") //  <  > \n"
  " ]\n"
  "[ phrase 14 { 6-7 } false `second (phrase_function=\" NA \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.7
QueryAnswer(false, "topographic query on realistic db number 20.7",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase as p1\n"
  "   [\n"
  "      [word parents HAS p1.self]\n"
  "   ]\n"
  "   [Word parents HAS p1.self]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.8
QueryAnswer(false, "topographic query on realistic db number 20.8",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase as p1\n"
  "   [\n"
  "      [word parents HAS p1.self]\n"
  "      OR\n"
  "      [Phrase self = p1.self]\n"
  "   ]\n"
  "   [Word parents HAS p1.self] * {0,1}\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 10 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false  //  <  < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false  //  <  < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 8 } false  //  <  < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 9 } false  //  <  < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.9
QueryAnswer(false, "topographic query on realistic db number 20.9",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [\n"
  "	[Word GET POS]\n"
  "        ..\n"
  "        [Word GET POS]\n"
  "   ] * { 1-3 }\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 1 { 1 } false (POS=\"def_article\") //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false (POS=\"noun\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false (POS=\"def_article\") //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false (POS=\"verb\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false (POS=\"def_article\") //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false (POS=\"adjective\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false (POS=\"noun\") //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false (POS=\"verb\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false (POS=\"noun\") //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false (POS=\"adjective\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false (POS=\"verb\") //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false (POS=\"adjective\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  < [ word 3 { 3 } false (POS=\"relative_pronoun\") //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false (POS=\"verb\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false (POS=\"relative_pronoun\") //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false (POS=\"preposition\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false (POS=\"relative_pronoun\") //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false (POS=\"def_article\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false (POS=\"relative_pronoun\") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (POS=\"noun\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false (POS=\"relative_pronoun\") //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false (POS=\"verb\") //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false (POS=\"preposition\") //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false (POS=\"def_article\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false (POS=\"relative_pronoun\") //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false (POS=\"verb\") //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false (POS=\"preposition\") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (POS=\"noun\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false (POS=\"relative_pronoun\") //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false (POS=\"preposition\") //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false (POS=\"def_article\") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (POS=\"noun\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false (POS=\"verb\") //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false (POS=\"preposition\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false (POS=\"verb\") //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false (POS=\"def_article\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false (POS=\"verb\") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (POS=\"noun\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false (POS=\"verb\") //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false (POS=\"preposition\") //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false (POS=\"def_article\") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (POS=\"noun\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false (POS=\"preposition\") //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false (POS=\"def_article\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false (POS=\"preposition\") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (POS=\"noun\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false (POS=\"def_article\") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (POS=\"noun\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.10
QueryAnswer(false, "topographic query on realistic db number 20.10",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]\n"
  "   [\n"
  "	[Phrase]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.11
QueryAnswer(false, "topographic query on realistic db number 20.11",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]\n"
  "   [\n"
  "	[Phrase]\n"
  "	[Phrase]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  "[ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.12
QueryAnswer(false, "topographic query on realistic db number 20.12",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]\n"
  "   [\n"
  "	[Phrase]\n"
  "	..\n"
  "	[Phrase]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  "[ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  "[ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.13
QueryAnswer(false, "topographic query on realistic db number 20.13",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]\n"
  "   [\n"
  "	[Phrase]\n"
  "   ]\n"
  "   [Word]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.14
QueryAnswer(false, "topographic query on realistic db number 20.14",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]\n"
  "   [\n"
  "	[Word]\n"
  "	[Word]\n"
  "   ]\n"
  "   [Word]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.15
QueryAnswer(false, "topographic query on realistic db number 20.15",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]\n"
  "   ..\n"
  "   [\n"
  "	[Phrase]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ phrase 13 { 5-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  "[ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  "[ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.16
QueryAnswer(false, "topographic query on realistic db number 20.16",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]\n"
  "   ..\n"
  "   [\n"
  "	[Phrase]\n"
  "   ]\n"
  "   ..\n"
  "   [Word]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  "[ phrase 12 { 4 } false  //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.17
QueryAnswer(false, "topographic query on realistic db number 20.17",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]\n"
  "   [\n"
  "	[Word]\n"
  "	..\n"
  "	[\n"
  "		[Phrase]\n"
  "	]\n"
  "   ]\n"
  "   ..\n"
  "   [Word]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  "[ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.18
QueryAnswer(false, "topographic query on realistic db number 20.18",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]!\n"
  "   [\n"
  "     [gap retrieve]\n"
  "     [word]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ pow_m { 3-7 } false  //  <  > \n"
  " ]\n"
  "[ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.19
QueryAnswer(false, "topographic query on realistic db number 20.19",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]!\n"
  "   [\n"
  "	[Word]\n"
  "	[Gap retrieve]\n"
  "	[Phrase]\n"
  "	[Phrase]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ pow_m { 3-7 } false  //  <  > \n"
  " ]\n"
  "[ phrase 15 { 8 } false  //  <  > \n"
  " ]\n"
  "[ phrase 16 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//topographic query on realistic db number 20.20
QueryAnswer(false, "topographic query on realistic db number 20.20",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [Word]!\n"
  "   [\n"
  "	[gap? retrieve]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 20 { 1-2, 8-9 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  "[ pow_m { 3-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 21 { 3-7 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//drop indexes on single object type
QueryAnswer(false, "drop indexes on single object type",
  "drop indexes on object type [Paragraph]\n"
  "",
  true, true,
  ""),


//create indexes on single object type
QueryAnswer(false, "create indexes on single object type",
  "create indexes on object type [Paragraph]\n"
  "",
  true, true,
  ""),


//drop indexes on all object types
QueryAnswer(false, "drop indexes on all object types",
  "drop indexes on object types [ALL]\n"
  "",
  true, true,
  ""),


//vacuum database without analyze
QueryAnswer(false, "vacuum database without analyze",
  "vacuum database\n"
  "",
  true, true,
  ""),


//create indexes on all object types
QueryAnswer(false, "create indexes on all object types",
  "create indexes on object types [ALL]\n"
  "",
  true, true,
  ""),


//vacuum database with analyze
QueryAnswer(false, "vacuum database with analyze",
  "vacuum database analyze\n"
  "",
  true, true,
  ""),


//create object type with extremely long name...
QueryAnswer(false, "create object type with extremely long name...",
  "CREATE OBJECT TYPE\n"
  "[otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah\n"
  "  MYINTEGER : INTEGER;\n"
  "  MYID_D : id_d;\n"
  "  MYASCII : ASCII;\n"
  "  MYSTRING : STRING;\n"
  "  MYSTRINGFROMSET : STRING FROM SET;\n"
  "  MYSOM : SET OF MONADS;\n"
  "  MYENUM : phrase_type_t;\n"
  "  MYLISTOFINTEGER : LIST OF INTEGER;\n"
  "  MYLISTOFID_D : LIST OF ID_D;\n"
  "  MYLISTOFENUM : LIST OF Phrase_type_t;\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  ""),


//creating one object (1) inside the long-named object type
QueryAnswer(false, "creating one object (1) inside the long-named object type",
  "CREATE OBJECT\n"
  "FROM MONADS = {1}\n"
  "[otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah\n"
  "  myinteger := 100;\n"
  "  myid_d := nil;\n"
  "  myascii := \"Blah\";\n"
  "  mystring := \"Blah blah\";\n"
  "  mystringfromset := \"BLAH\";\n"
  "  mysom := { 1-10 };\n"
  "  myenum := NP;\n"
  "  mylistofinteger := (1,2,3);\n"
  "  mylistofid_d := (1,2,3);\n"
  "  mylistofenum := (VP,NP);\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 24          |\n"
  "+-------------+\n"
  ""),


//creating one object (2) inside the long-named object type
QueryAnswer(false, "creating one object (2) inside the long-named object type",
  "CREATE OBJECT\n"
  "FROM MONADS = {2}\n"
  "[otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah\n"
  "  myinteger := 101;\n"
  "  myid_d := nil;\n"
  "  myascii := \"Blah2\";\n"
  "  mystring := \"Blah2 blah2\";\n"
  "  mystringfromset := \"BLAH2\";\n"
  "  mysom := { 1-12 };\n"
  "  myenum := VP;\n"
  "  mylistofinteger := (1,2,3,4000000);\n"
  "  mylistofid_d := (1,2,3,3000000);\n"
  "  mylistofenum := (NP,VP);\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 25          |\n"
  "+-------------+\n"
  ""),


//creating more than one object inside the long-named object type
QueryAnswer(false, "creating more than one object inside the long-named object type",
  "CREATE OBJECTS WITH OBJECT TYPE [otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {3}\n"
  "[\n"
  "  myinteger := 100;\n"
  "  myid_d := nil;\n"
  "  myascii := \"Blah\";\n"
  "  mystring := \"Blah blah\";\n"
  "  mystringfromset := \"BLAH\";\n"
  "  mysom := { 1-10 };\n"
  "  myenum := NP;\n"
  "  mylistofinteger := (1,2,3);\n"
  "  mylistofid_d := (1,2,3);\n"
  "  mylistofenum := ();\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {4}\n"
  "[\n"
  "  myinteger := 100;\n"
  "  myid_d := nil;\n"
  "  myascii := \"Blah\";\n"
  "  mystring := \"Blah blah\";\n"
  "  mystringfromset := \"BLAH\";\n"
  "  mysom := { 4 };\n"
  "  myenum := NP;\n"
  "  mylistofinteger := (1,2,3);\n"
  "  mylistofid_d := ();\n"
  "  mylistofenum := (VP);\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 2                      |\n"
  "+------------------------+\n"
  ""),


//REcreating realistic DB schema
QueryAnswer(true, "REcreating realistic DB schema",
  "CREATE ENUMERATION pos_t = {\n"
  "	def_article, noun, verb, relative_pronoun, preposition, adjective\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE ENUMERATION phrase_type_t = {\n"
  "   NP, VP, PP, AP\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE ENUMERATION phrase_function_t = {\n"
  "   NA, Subj, Objc, Rela, Pred, Loca, PreC, Attr\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE MONAD OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[Word\n"
  "   surface: STRING FROM SET;\n"
  "   pos : pos_t;\n"
  "   parent : id_d;\n"
  "   lemma : STRING FROM SET WITH INDEX;\n"
  "   parents : LIST OF id_d;\n"
  "   magic : integer DEFAULT 42;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH MULTIPLE RANGE OBJECTS\n"
  "WITHOUT UNIQUE MONADS\n"
  "[Phrase\n"
  "  phrase_type : phrase_type_t;\n"
  "  phrase_function : LIST OF phrase_function_t;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[Clause_atom]\n"
  "GO\n"
  "\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH MULTIPLE RANGE OBJECTS\n"
  "WITHOUT UNIQUE MONADS\n"
  "[Clause]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH MULTIPLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[Sentence]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST AND LAST MONADS\n"
  "[Paragraph]\n"
  "GO\n"
  "",
  true, true,
  ""),


//UPDATING object type Phrase
QueryAnswer(false, "UPDATING object type Phrase",
  "UPDATE OBJECT TYPE\n"
  "[Phrase\n"
  "   REMOVE phrase_function;\n"
  "   ADD surface: STRING FROM SET;\n"
  "   ADD pos : pos_t;\n"
  "   ADD parent : id_d;\n"
  "   ADD lemma : STRING FROM SET WITH INDEX;\n"
  "   ADD parents : LIST OF id_d;\n"
  "   ADD magic : integer DEFAULT 42;\n"
  "]\n"
  "",
  true, true,
  ""),


//selecting features from newly updated Phrase object type
QueryAnswer(false, "selecting features from newly updated Phrase object type",
  "SELECT FEATURES FROM TYPE [Phrase]\n"
  "",
  true, true,
  "+-----------------------+---------------+------------------+-----------------+\n"
  "| feature name : string | type : string | default : string | computed : bool |\n"
  "+-----------------------+---------------+------------------+-----------------+\n"
  "| phrase_type           | phrase_type_t | 'NP'             | false           |\n"
  "| self                  | id_d          | '0'              | true            |\n"
  "| surface               | string        | ''               | false           |\n"
  "| pos                   | pos_t         | 'def_article'    | false           |\n"
  "| parent                | id_d          | '0'              | false           |\n"
  "| lemma                 | string        | ''               | false           |\n"
  "| parents               | list of id_d  | ''               | false           |\n"
  "| magic                 | integer       | '42'             | false           |\n"
  "+-----------------------+---------------+------------------+-----------------+\n"
  ""),


//UPDATING object type Word, removing all features
QueryAnswer(false, "UPDATING object type Word, removing all features",
  "UPDATE TYPE\n"
  "[Word\n"
  "   REMOVE surface;\n"
  "   REMOVE pos;\n"
  "   REMOVE parent;\n"
  "   REMOVE lemma;\n"
  "   REMOVE parents;\n"
  "   REMOVE magic;\n"
  "]\n"
  "",
  true, true,
  ""),


//selecting features from newly updated Word object type
QueryAnswer(false, "selecting features from newly updated Word object type",
  "SELECT FEATURES FROM TYPE [Word]\n"
  "",
  true, true,
  "+-----------------------+---------------+------------------+-----------------+\n"
  "| feature name : string | type : string | default : string | computed : bool |\n"
  "+-----------------------+---------------+------------------+-----------------+\n"
  "| self                  | id_d          | '0'              | true            |\n"
  "+-----------------------+---------------+------------------+-----------------+\n"
  ""),


//UPDATING object type Word, adding a lot of features...
QueryAnswer(false, "UPDATING object type Word, adding a lot of features...",
  "UPDATE TYPE\n"
  "[Word\n"
  "    ADD myinteger : INTEGER DEFAULT 23;\n"
  "    ADD myintegerwithindex : INTEGER WITH INDEX DEFAULT -133;\n"
  "    ADD myid_d : ID_D DEFAULT 2;\n"
  "    ADD myid_dwithindex : ID_D WITH INDEX DEFAULT 1;\n"
  "    ADD mystring : STRING DEFAULT \"blah\";\n"
  "    ADD mystringwithindex : STRING WITH INDEX DEFAULT \"blof\";\n"
  "    ADD mystringfromset : STRING FROM SET DEFAULT \"bloat\";\n"
  "    ADD mystringfromsetwithindex : STRING FROM SET WITH INDEX DEFAULT \"float\";\n"
  "    ADD myascii : ASCII DEFAULT \"blah\";\n"
  "    ADD myasciiwithindex : ASCII WITH INDEX DEFAULT \"blof\";\n"
  "    ADD myasciifromset : ASCII FROM SET DEFAULT \"bloat\";\n"
  "    ADD myasciifromsetwithindex : ASCII FROM SET WITH INDEX DEFAULT \"float\";\n"
  "    ADD myphrasefunction : Phrase_Function_T DEFAULT Subj;\n"
  "    ADD mylistofinteger : LIST OF INTEGER;\n"
  "    ADD mylistofid_d : LIST OF ID_D;\n"
  "    ADD mylistofenum : LIST OF Phrase_Function_T;\n"
  "]\n"
  "",
  true, true,
  ""),


//selecting features from newly updated Word object type
QueryAnswer(false, "selecting features from newly updated Word object type",
  "SELECT FEATURES FROM TYPE [Word]\n"
  "",
  true, true,
  "+--------------------------+---------------------------+------------------+-----------------+\n"
  "| feature name : string    | type : string             | default : string | computed : bool |\n"
  "+--------------------------+---------------------------+------------------+-----------------+\n"
  "| self                     | id_d                      | '0'              | true            |\n"
  "| myinteger                | integer                   | '23'             | false           |\n"
  "| myintegerwithindex       | integer                   | '-133'           | false           |\n"
  "| myid_d                   | id_d                      | '2'              | false           |\n"
  "| myid_dwithindex          | id_d                      | '1'              | false           |\n"
  "| mystring                 | string                    | 'blah'           | false           |\n"
  "| mystringwithindex        | string                    | 'blof'           | false           |\n"
  "| mystringfromset          | string                    | 'bloat'          | false           |\n"
  "| mystringfromsetwithindex | string                    | 'float'          | false           |\n"
  "| myascii                  | ascii                     | 'blah'           | false           |\n"
  "| myasciiwithindex         | ascii                     | 'blof'           | false           |\n"
  "| myasciifromset           | ascii                     | 'bloat'          | false           |\n"
  "| myasciifromsetwithindex  | ascii                     | 'float'          | false           |\n"
  "| myphrasefunction         | phrase_function_t         | 'Subj'           | false           |\n"
  "| mylistofinteger          | list of integer           | ''               | false           |\n"
  "| mylistofid_d             | list of id_d              | ''               | false           |\n"
  "| mylistofenum             | list of phrase_function_t | ''               | false           |\n"
  "+--------------------------+---------------------------+------------------+-----------------+\n"
  ""),


//UPDATE OBJECT TYPE fail: Cannot assign number less than 0 to id_d.
QueryAnswer(false, "UPDATE OBJECT TYPE fail: Cannot assign number less than 0 to id_d.",
  "UPDATE OBJECT TYPE\n"
  "[Paragraph\n"
  "   ADD myid_d : id_d DEFAULT -1;\n"
  "]\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "You cannot assign a number below 0 to an id_d.\n"
  "Feature myid_d is not type-compatible with its default value.\n"
  ""),


//UPDATE OBJECT TYPE fail: Cannot REMOVE self.
QueryAnswer(false, "UPDATE OBJECT TYPE fail: Cannot REMOVE self.",
  "UPDATE OBJECT TYPE\n"
  "[Paragraph\n"
  "   REMOVE self;\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Cannot remove feature 'self'.\n"
  ""),


//UPDATE OBJECT TYPE fail: Cannot ADD self.
QueryAnswer(false, "UPDATE OBJECT TYPE fail: Cannot ADD self.",
  "UPDATE OBJECT TYPE\n"
  "[Paragraph\n"
  "   ADD self : STRING from set;\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Feature 'self' is implicitly declared and cannot be declared explicitly.\n"
  ""),


//CREATE OBJECT TYPE fail: Cannot declare self.
QueryAnswer(false, "CREATE OBJECT TYPE fail: Cannot declare self.",
  "CREATE OBJECT TYPE\n"
  "[WillNotBeCreated\n"
  "   self : STRING from set;\n"
  "]\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Feature 'self' is implicitly declared and cannot be declared explicitly.\n"
  ""),


//UPDATE OBJECT TYPE fail: Cannot add feature with non-existent enum
QueryAnswer(false, "UPDATE OBJECT TYPE fail: Cannot add feature with non-existent enum",
  "UPDATE OBJECT TYPE\n"
  "[Paragraph\n"
  "   ADD featurewillnotbeadded : enumwhichdoesnotexist_t;\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration enumwhichdoesnotexist_t used as the type of feature featurewillnotbeadded does not exist.\n"
  ""),


//CREATE OBJECT TYPE fail: Cannot add feature with non-existent enum
QueryAnswer(false, "CREATE OBJECT TYPE fail: Cannot add feature with non-existent enum",
  "CREATE OBJECT TYPE\n"
  "[WillNotBeCreated\n"
  "   featurewillnotbecreated : enumwhichdoesnotexist_t;\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration enumwhichdoesnotexist_t used as the type of feature featurewillnotbecreated does not exist.\n"
  ""),


//UPDATE OBJECT TYPE fail: Cannot add feature with non-existent enum constant
QueryAnswer(false, "UPDATE OBJECT TYPE fail: Cannot add feature with non-existent enum constant",
  "UPDATE OBJECT TYPE\n"
  "[Paragraph\n"
  "   ADD featurewillnotbeadded : Phrase_Type_t DEFAULT NotApplicable;\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration constant NotApplicable does not exist in the enumeration Phrase_Type_t used as the type of feature featurewillnotbeadded.\n"
  ""),


//DELETE OBJECTS from an object type with no objects
QueryAnswer(false, "DELETE OBJECTS from an object type with no objects",
  "DELETE OBJECTS \n"
  "BY MONADS = { 1 - 210000000 }\n"
  "[Paragraph]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "+-------------+\n"
  ""),


//CREATE OBJECT TYPE fail: Cannot add feature with non-existent enum constant
QueryAnswer(false, "CREATE OBJECT TYPE fail: Cannot add feature with non-existent enum constant",
  "CREATE OBJECT TYPE\n"
  "[WillNotBeCreated\n"
  "   featurewillnotbeadded : Phrase_Type_t DEFAULT NotApplicable;\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration constant NotApplicable does not exist in the enumeration Phrase_Type_t used as the type of feature featurewillnotbeadded.\n"
  ""),


//UPDATE OBJECT TYPE fail: Cannot add feature which exists already
QueryAnswer(false, "UPDATE OBJECT TYPE fail: Cannot add feature which exists already",
  "UPDATE OBJECT TYPE\n"
  "[Word\n"
  "    ADD myinteger : INTEGER DEFAULT 150;\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Feature myinteger exists already.  Cannot add again.\n"
  ""),


//DROP OBJECT TYPE fail: Cannot remove non-existent object type
QueryAnswer(false, "DROP OBJECT TYPE fail: Cannot remove non-existent object type",
  "DROP TYPE [DoesNotExist]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Object type DoesNotExist does not exist.\n"
  ""),


//UPDATE OBJECT TYPE fail: Cannot update non-existent object type
QueryAnswer(false, "UPDATE OBJECT TYPE fail: Cannot update non-existent object type",
  "UPDATE TYPE [DoesNotExist\n"
  "	ADD willnotbeadded : integer;\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Object type DoesNotExist does not exist.\n"
  ""),


//CREATE OBJECT TYPE fail: Cannot create already-existent object type
QueryAnswer(false, "CREATE OBJECT TYPE fail: Cannot create already-existent object type",
  "CREATE TYPE [Word]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Object type Word exists already.\n"
  ""),


//CREATE ENUMERATION fail: Cannot have two defaults
QueryAnswer(false, "CREATE ENUMERATION fail: Cannot have two defaults",
  "CREATE ENUMERATION will_not_be_created_t = {\n"
  "   DEFAULT blah = 1,\n"
  "   DEFAULT blof = 2\n"
  "}\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "You cannot have more than one default\n"
  ""),


//CREATE ENUMERATION fail: Cannot have two constants with the same name
QueryAnswer(false, "CREATE ENUMERATION fail: Cannot have two constants with the same name",
  "CREATE ENUMERATION will_not_be_created_t = {\n"
  "   blah = 1,\n"
  "   blah = 2\n"
  "}\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration constant blah is declared more than once.\n"
  "This is not allowed.\n"
  ""),


//CREATE ENUMERATION fail: Cannot have two constants with the same value
QueryAnswer(false, "CREATE ENUMERATION fail: Cannot have two constants with the same value",
  "CREATE ENUMERATION will_not_be_created_t = {\n"
  "   blah = 1,\n"
  "   blof = 1\n"
  "}\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Value 1 is assigned to more than one label.\n"
  "This is not allowed.\n"
  ""),


//CREATE ENUMERATION enumtest_t
QueryAnswer(false, "CREATE ENUMERATION enumtest_t",
  "CREATE ENUMERATION enumtest_t = {\n"
  "    DEFAULT one = 1,\n"
  "    two = 2,\n"
  "    three = 3\n"
  "}\n"
  "",
  true, true,
  ""),


//UPDATE ENUMERATION enumtest_t: Cannot remove default without specifying new default
QueryAnswer(false, "UPDATE ENUMERATION enumtest_t: Cannot remove default without specifying new default",
  "UPDATE ENUMERATION enumtest_t = {\n"
  "    REMOVE one\n"
  "}\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Default is removed without specifying new default.\n"
  ""),


//UPDATE ENUMERATION enumtest_t fail: Cannot have more than one default: ADD ADD
QueryAnswer(false, "UPDATE ENUMERATION enumtest_t fail: Cannot have more than one default: ADD ADD",
  "UPDATE ENUMERATION enumtest_t = {\n"
  "    ADD DEFAULT four = 4,\n"
  "    ADD DEFAULT five = 5\n"
  "}\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "You cannot have more than one default\n"
  ""),


//UPDATE ENUMERATION enumtest_t fail: Cannot have more than one default: ADD UPDATE
QueryAnswer(false, "UPDATE ENUMERATION enumtest_t fail: Cannot have more than one default: ADD UPDATE",
  "UPDATE ENUMERATION enumtest_t = {\n"
  "    ADD DEFAULT four = 4,\n"
  "    UPDATE DEFAULT three = 5\n"
  "}\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "You cannot have more than one default\n"
  ""),


//UPDATE ENUMERATION enumtest_t fail: Cannot add already-existing enum constant
QueryAnswer(false, "UPDATE ENUMERATION enumtest_t fail: Cannot add already-existing enum constant",
  "UPDATE ENUMERATION enumtest_t = {\n"
  "    ADD three = 4\n"
  "}\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration constant three exists already.  Cannot add.\n"
  ""),


//UPDATE ENUMERATION enumtest_t fail: Cannot update non-existing enum constant
QueryAnswer(false, "UPDATE ENUMERATION enumtest_t fail: Cannot update non-existing enum constant",
  "UPDATE ENUMERATION enumtest_t = {\n"
  "    UPDATE DEFAULT ten = 10\n"
  "}\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration constant ten does not exist.  Cannot update.\n"
  ""),


//UPDATE ENUMERATION enumtest_t fail: Cannot remove non-existing enum constant
QueryAnswer(false, "UPDATE ENUMERATION enumtest_t fail: Cannot remove non-existing enum constant",
  "UPDATE ENUMERATION enumtest_t = {\n"
  "    REMOVE ten\n"
  "}\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration constant ten does not exist.  Cannot remove.\n"
  ""),


//UPDATE ENUMERATION enumtest_t success
QueryAnswer(false, "UPDATE ENUMERATION enumtest_t success",
  "UPDATE ENUMERATION enumtest_t = {\n"
  "    REMOVE one,\n"
  "    ADD DEFAULT four = 4,\n"
  "    UPDATE three = 5\n"
  "}\n"
  "",
  true, true,
  ""),


//SELECT ENUMERATION CONSTANTS enumtest_t
QueryAnswer(false, "SELECT ENUMERATION CONSTANTS enumtest_t",
  "SELECT ENUMERATION CONSTANTS FROM ENUMERATION enumtest_t\n"
  "",
  true, true,
  "+---------------------+-----------------+----------------+\n"
  "| enum const : string | value : integer | default : bool |\n"
  "+---------------------+-----------------+----------------+\n"
  "| four                | 4               | true           |\n"
  "| three               | 5               | false          |\n"
  "| two                 | 2               | false          |\n"
  "+---------------------+-----------------+----------------+\n"
  ""),


//DROP ENUMERATION fail: enum must exist
QueryAnswer(false, "DROP ENUMERATION fail: enum must exist",
  "DROP ENUMERATION DoesNotExist_t\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration DoesNotExist_t did not exist.\n"
  ""),


//DROP ENUMERATION enumtest_t
QueryAnswer(false, "DROP ENUMERATION enumtest_t",
  "DROP ENUMERATION enumtest_t\n"
  "",
  true, true,
  ""),


//SELECT ENUMERATION CONSTANTS enumtest_t fail: Does not exist
QueryAnswer(false, "SELECT ENUMERATION CONSTANTS enumtest_t fail: Does not exist",
  "SELECT ENUMERATION CONSTANTS FROM ENUMERATION enumtest_t\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "Enumeration enumtest_t did not exist.\n"
  ""),


//CREATE SEGMENT
QueryAnswer(false, "CREATE SEGMENT",
  "CREATE SEGMENT one_five RANGE = 1 - 5\n"
  "",
  true, true,
  ""),


//CREATE SEGMENT fail: Cannot create backwards range
QueryAnswer(false, "CREATE SEGMENT fail: Cannot create backwards range",
  "CREATE SEGMENT five_one RANGE = 5 - 1\n"
  "",
  true, false,
  "Weeding failed with compiler error\n"
  "Range in monad set must be monotonic (i.e., in 'A - B',\n"
  "B must be greater than or equal to A).\n"
  "The offending mse was 5-1.\n"
  ""),


//CREATE SEGMENT fail: Cannot create existing segment
QueryAnswer(false, "CREATE SEGMENT fail: Cannot create existing segment",
  "CREATE SEGMENT one_five RANGE = 1 - 6\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "monad set with name 'one_five' exists already.\n"
  "Cannot create.\n"
  ""),


//CREATE OBJECT TYPE foobar
QueryAnswer(false, "CREATE OBJECT TYPE foobar",
  "CREATE OBJECT TYPE [foobar] GO\n"
  "",
  true, true,
  ""),


//DELETE OBJECTS on foobar, which has no objects
QueryAnswer(false, "DELETE OBJECTS on foobar, which has no objects",
  "DELETE OBJECTS BY MONADS = { 1-10 } [foobar] GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "+-------------+\n"
  ""),


//creating small database for testing object reference machinery
QueryAnswer(true, "creating small database for testing object reference machinery",
  "CREATE OBJECT TYPE [Phrase] GO\n"
  "CREATE OBJECT TYPE [Word psp : INTEGER;] GO\n"
  "CREATE OBJECT FROM MONADS = {1} [Word psp := 5;] GO\n"
  "CREATE OBJECT FROM MONADS = {2} [Word psp := 0;] GO\n"
  "CREATE OBJECT FROM MONADS = {3} [Word psp := 1;] GO\n"
  "CREATE OBJECT FROM MONADS = {1-3} [Phrase]GO\n"
  "CREATE OBJECT FROM MONADS = {4} [Word psp := 0;] GO\n"
  "CREATE OBJECT FROM MONADS = {5} [Word psp := 1;] GO\n"
  "CREATE OBJECT FROM MONADS = {4-5} [Phrase] GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 7           |\n"
  "+-------------+\n"
  ""),


//testing the object reference machinery #1
QueryAnswer(false, "testing the object reference machinery #1",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Phrase\n"
  "  [Word psp=5]\n"
  "  ..\n"
  "  [Word AS w1 psp=0 OR psp=1]\n"
  "]\n"
  "[Phrase\n"
  "  [Word psp=w1.psp]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 4 { 1-3 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ phrase 7 { 4-5 } false  //  <  < [ word 5 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//creating small database for testing object reference machinery #2
QueryAnswer(true, "creating small database for testing object reference machinery #2",
  "CREATE ENUMERATION pos_t = {\n"
  "       V,\n"
  "       N,\n"
  "       P,\n"
  "       A,\n"
  "       PRON,\n"
  "       NEG\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "[Sentence]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "[Clause\n"
  "   mother : id_d;\n"
  "   parent : id_d;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "[Phrase\n"
  "    parent : id_d;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE \n"
  "WITH SINGLE MONAD OBJECTS\n"
  "[Word\n"
  "    surface : STRING;\n"
  "    pos : pos_t;\n"
  "    myint : INTEGER;\n"
  "    parent : id_d;\n"
  "    mother : id_d;\n"
  "]\n"
  "GO\n"
  "\n"
  "\n"
  "\n"
  "\n"
  "\n"
  "CREATE OBJECT FROM MONADS = {1-10}\n"
  "WITH ID_D = 16\n"
  "[Sentence]\n"
  "GO\n"
  "\n"
  "\n"
  "CREATE OBJECT FROM MONADS = {1-3}\n"
  "WITH ID_D = 4\n"
  "[Clause\n"
  "   mother := NIL;\n"
  "   parent := 16;\n"
  "]\n"
  "GO\n"
  "\n"
  "\n"
  "\n"
  "CREATE OBJECTS WITH OBJECT TYPE [Phrase]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {1}\n"
  "WITH ID_D = 10\n"
  "[\n"
  "    parent := 4;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {2}\n"
  "WITH ID_D = 11\n"
  "[\n"
  "    parent := 4;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {3}\n"
  "WITH ID_D = 12\n"
  "[\n"
  "    parent := 4;\n"
  "]\n"
  "GO\n"
  "\n"
  "\n"
  "CREATE OBJECTS WITH OBJECT TYPE [Word]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {1}\n"
  "WITH ID_D = 1\n"
  "[\n"
  "    surface := \"I\";\n"
  "    pos := PRON;\n"
  "    myint := 1;\n"
  "    parent := 10;\n"
  "    mother := NIL;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {2}\n"
  "WITH ID_D = 2\n"
  "[\n"
  "    surface := \"am\";\n"
  "    pos := V;\n"
  "    myint := 2;\n"
  "    parent := 11;\n"
  "    mother := 1;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {3}\n"
  "WITH ID_D = 3\n"
  "[\n"
  "    surface := \"he.\";\n"
  "    pos := PRON;\n"
  "    myint := 3;\n"
  "    parent := 12;\n"
  "    mother := 2;\n"
  "]\n"
  "GO\n"
  "\n"
  "\n"
  "\n"
  "\n"
  "\n"
  "\n"
  "\n"
  "\n"
  "CREATE OBJECT FROM MONADS = {4-7}\n"
  "WITH ID_D = 9\n"
  "[Clause\n"
  "   mother := 4;\n"
  "   parent := 16;\n"
  "]\n"
  "GO\n"
  "\n"
  "\n"
  "CREATE OBJECTS WITH OBJECT TYPE [Phrase]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {4}\n"
  "WITH ID_D = 13\n"
  "[\n"
  "    parent := 9;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {5}\n"
  "WITH ID_D = 14\n"
  "[\n"
  "    parent := 9;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {6-7}\n"
  "WITH ID_D = 15\n"
  "[\n"
  "    parent := 9;\n"
  "]\n"
  "GO\n"
  "\n"
  "\n"
  "CREATE OBJECTS WITH OBJECT TYPE [Word]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {4}\n"
  "WITH ID_D = 5\n"
  "[\n"
  "    surface := \"He\";\n"
  "    pos := PRON;\n"
  "    myint := 1;\n"
  "    parent := 13;\n"
  "    mother := 10;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {5}\n"
  "WITH ID_D = 6\n"
  "[\n"
  "    surface := \"is\";\n"
  "    pos := V;\n"
  "    myint := 2;\n"
  "    parent := 14;\n"
  "    mother := 11;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {6}\n"
  "WITH ID_D = 7\n"
  "[\n"
  "    surface := \"not\";\n"
  "    pos := NEG;\n"
  "    myint := 1;\n"
  "    parent := 15;\n"
  "    mother := 12;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {7}\n"
  "WITH ID_D = 8\n"
  "[\n"
  "    surface := \"she.\";\n"
  "    pos := PRON;\n"
  "    myint := 1;\n"
  "    parent := 15;\n"
  "    mother := 4;\n"
  "]\n"
  "GO\n"
  "\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 4                      |\n"
  "+------------------------+\n"
  ""),


//testing object reference machinery #2
QueryAnswer(false, "testing object reference machinery #2",
  "SELECT ALL OBJECTS IN { 1-7 }\n"
  "WHERE \n"
  "[Clause\n"
  "  [Word AS w1 GET pos]\n"
  "]\n"
  "[Clause\n"
  "  [Word pos=w1.pos AND myint=1 GET myint, pos]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  < [ word 1 { 1 } false (pos=\"PRON\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false (pos=\"PRON\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 9 { 4-7 } false  //  <  < [ word 5 { 4 } false (myint=\"1\",pos=\"PRON\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 7 } false (myint=\"1\",pos=\"PRON\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//testing object reference machinery #3
QueryAnswer(false, "testing object reference machinery #3",
  "SELECT ALL OBJECTS \n"
  "WHERE \n"
  "[Clause\n"
  "  [Word AS w1]\n"
  "]\n"
  "[Clause\n"
  "  [Word pos=w1.pos AND myint=2 GET pos,myint]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 9 { 4-7 } false  //  <  < [ word 6 { 5 } false (pos=\"V\",myint=\"2\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//testing object reference machinery #4
QueryAnswer(false, "testing object reference machinery #4",
  "SELECT ALL OBJECTS IN {1-7}\n"
  "WHERE \n"
  "[Clause as c1\n"
  "  [Phrase as p1\n"
  "    parent = c1.self\n"
  "    [Word as w1\n"
  "        parent = p1.self\n"
  "    ]\n"
  "  ]\n"
  "]\n"
  "[Clause\n"
  "  [Phrase\n"
  "    [Word]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  < [ phrase 10 { 1 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 2 } false  //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 3 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 9 { 4-7 } false  //  <  < [ phrase 13 { 4 } false  //  <  < [ word 5 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 5 } false  //  <  < [ word 6 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 6-7 } false  //  <  < [ word 7 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//testing object reference machinery #5
QueryAnswer(false, "testing object reference machinery #5",
  "SELECT ALL OBJECTS IN {1-7}\n"
  "WHERE \n"
  "[Clause as c1\n"
  "  [Phrase as p1\n"
  "    parent = c1.self\n"
  "    [Word AS w1\n"
  "       parent = p1.self\n"
  "       GET parent\n"
  "    ]\n"
  "  ]\n"
  "]\n"
  "[Clause as c2\n"
  "  [Phrase as p2\n"
  "    parent = c2.self\n"
  "    [Word AS w2\n"
  "       parent = p2.self AND pos = w1.pos\n"
  "    ]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  < [ phrase 10 { 1 } false  //  <  < [ word 1 { 1 } false (parent=\"10\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 2 } false  //  <  < [ word 2 { 2 } false (parent=\"11\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 3 } false  //  <  < [ word 3 { 3 } false (parent=\"12\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 9 { 4-7 } false  //  <  < [ phrase 13 { 4 } false  //  <  < [ word 5 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 5 } false  //  <  < [ word 6 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 6-7 } false  //  <  < [ word 8 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//testing object reference machinery #6
QueryAnswer(false, "testing object reference machinery #6",
  "SELECT ALL OBJECTS IN {1-7}\n"
  "WHERE \n"
  "[\n"
  "  [\n"
  "    [Clause as c1]\n"
  "  ]\n"
  "]\n"
  "[Clause as c2\n"
  "  mother = c1.self\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  > \n"
  " ]\n"
  "[ clause 9 { 4-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//testing object reference machinery #7
QueryAnswer(false, "testing object reference machinery #7",
  "SELECT ALL OBJECTS IN {1-7}\n"
  "WHERE \n"
  "[\n"
  "  [\n"
  "    [Clause as c1]\n"
  "  ]\n"
  "]*\n"
  "[Clause as c2\n"
  "  mother = c1.self\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'c1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//testing object reference machinery #8
QueryAnswer(false, "testing object reference machinery #8",
  "SELECT ALL OBJECTS IN {1-7}\n"
  "WHERE \n"
  "[\n"
  "  [\n"
  "    [Clause as c1]\n"
  "  ]\n"
  "]\n"
  "OR\n"
  "[Clause as c2\n"
  "  mother = c1.self\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'c1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//testing object reference machinery #9
QueryAnswer(false, "testing object reference machinery #9",
  "SELECT ALL OBJECTS IN {1-7}\n"
  "WHERE \n"
  "[\n"
  "  [\n"
  "    [Clause as c1\n"
  "       [phrase as p1\n"
  "         [word as w1]\n"
  "       ]\n"
  "    ]\n"
  "  ]\n"
  "]\n"
  "[Clause as c2\n"
  "  mother = c1.self\n"
  "  [phrase as p2\n"
  "     parent = c2.self\n"
  "     [word as w2\n"
  "        parent = p2.self AND mother = p1.self AND pos = w1.pos\n"
  "     ]\n"
  "  ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  < [ phrase 10 { 1 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 2 } false  //  <  < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 3 } false  //  <  < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 9 { 4-7 } false  //  <  < [ phrase 13 { 4 } false  //  <  < [ word 5 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 5 } false  //  <  < [ word 6 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//testing object reference machinery #10
QueryAnswer(false, "testing object reference machinery #10",
  "SELECT ALL OBJECTS IN {1-7}\n"
  "WHERE \n"
  "[\n"
  "   [Clause as c1\n"
  "      [Word as w1 GET pos]\n"
  "      ..\n"
  "      [word as w2 GET pos]\n"
  "   ]\n"
  "]\n"
  "[Clause\n"
  "   [Word pos = w1.pos GET pos]\n"
  "   ..\n"
  "   [Word pos = w2.pos GET pos]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  < [ word 1 { 1 } false (pos=\"PRON\") //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 1 { 1 } false (pos=\"PRON\") //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false (pos=\"PRON\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 3 { 3 } false (pos=\"PRON\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 9 { 4-7 } false  //  <  < [ word 5 { 4 } false (pos=\"PRON\") //  <  > \n"
  " ]\n"
  "[ word 6 { 5 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 4 } false (pos=\"PRON\") //  <  > \n"
  " ]\n"
  "[ word 8 { 7 } false (pos=\"PRON\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 5 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 8 { 7 } false (pos=\"PRON\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//creating more objects for testing object reference machinery
QueryAnswer(false, "creating more objects for testing object reference machinery",
  "CREATE OBJECT FROM MONADS = {8-10}\n"
  "WITH ID_D = 17\n"
  "[Clause\n"
  "   mother := 9;\n"
  "   parent := 16;\n"
  "]\n"
  "GO\n"
  "\n"
  "\n"
  "CREATE OBJECTS WITH OBJECT TYPE [Phrase]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {8-9}\n"
  "WITH ID_D = 18\n"
  "[\n"
  "    parent := 17;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {10}\n"
  "WITH ID_D = 19\n"
  "[\n"
  "    parent := 17;\n"
  "]\n"
  "GO\n"
  "\n"
  "\n"
  "CREATE OBJECTS WITH OBJECT TYPE [Word]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {8}\n"
  "WITH ID_D = 20\n"
  "[\n"
  "    surface := \"He\";\n"
  "    pos := PRON;\n"
  "    myint := 1;\n"
  "    parent := 18;\n"
  "    mother := 10;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {9}\n"
  "WITH ID_D = 21\n"
  "[\n"
  "    surface := \"is\";\n"
  "    pos := V;\n"
  "    myint := 2;\n"
  "    parent := 18;\n"
  "    mother := 11;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {10}\n"
  "WITH ID_D = 23\n"
  "[\n"
  "    surface := \"not\";\n"
  "    pos := NEG;\n"
  "    myint := 1;\n"
  "    parent := 19;\n"
  "    mother := 12;\n"
  "]\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 3                      |\n"
  "+------------------------+\n"
  ""),


//doing extended querying for object references #1
QueryAnswer(false, "doing extended querying for object references #1",
  "SELECT ALL OBJECTS IN {1-10}\n"
  "WHERE \n"
  "[\n"
  "   [Clause as c1\n"
  "      [Word as w1]\n"
  "   ]\n"
  "]\n"
  "[Clause as c2\n"
  "   [Word as w2 pos <> w1.pos]\n"
  "]\n"
  "[Clause as c3\n"
  "   [Phrase as p1\n"
  "      [Word (pos = w1.pos AND pos = w2.pos) AND mother = w2.mother]\n"
  "   ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 9 { 4-7 } false  //  <  < [ word 5 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 5 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 17 { 8-10 } false  //  <  < [ phrase 18 { 8-9 } false  //  <  < [ word 20 { 8 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 21 { 9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//doing extended querying for object references #2
QueryAnswer(false, "doing extended querying for object references #2",
  "SELECT ALL OBJECTS IN {1-10}\n"
  "WHERE \n"
  "[\n"
  "   [Clause as c1\n"
  "      [Word as w1]\n"
  "   ]\n"
  "]\n"
  "[Clause as c2\n"
  "   [Word as w2 pos = NEG]\n"
  "]\n"
  "[Clause as c3\n"
  "   [Phrase as p1\n"
  "      [Word (pos = w1.pos OR pos = w2.pos) AND mother = w2.mother]\n"
  "   ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 9 { 4-7 } false  //  <  < [ word 7 { 6 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 17 { 8-10 } false  //  <  < [ phrase 19 { 10 } false  //  <  < [ word 23 { 10 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//doing extended querying for object references #3
QueryAnswer(false, "doing extended querying for object references #3",
  "SELECT ALL OBJECTS IN {1-10}\n"
  "WHERE \n"
  "[\n"
  "   [Clause as c1\n"
  "      [Word as w1 GET pos, mother]\n"
  "   ]\n"
  "]\n"
  "..\n"
  "[Clause as c2\n"
  "   [Phrase as p1 \n"
  "      parent = c2.self\n"
  "      [Word pos = w1.pos AND mother = w1.mother GET pos, mother]\n"
  "   ]\n"
  "]\n"
  "",
  true, true,
  " //  <  < [ clause 9 { 4-7 } false  //  <  < [ word 5 { 4 } false (pos=\"PRON\",mother=\"10\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 5 } false (pos=\"V\",mother=\"11\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 6 } false (pos=\"NEG\",mother=\"12\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 7 } false (pos=\"PRON\",mother=\"4\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  "[ clause 17 { 8-10 } false  //  <  < [ phrase 18 { 8-9 } false  //  <  < [ word 20 { 8 } false (pos=\"PRON\",mother=\"10\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 21 { 9 } false (pos=\"V\",mother=\"11\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 19 { 10 } false  //  <  < [ word 23 { 10 } false (pos=\"NEG\",mother=\"12\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//doing WITH MAX RANGE 3 MONADS
QueryAnswer(false, "doing WITH MAX RANGE 3 MONADS",
  "SELECT ALL OBJECTS IN {1-10}\n"
  "WITH MAX RANGE 3 MONADS\n"
  "WHERE \n"
  "[Word AS w1 pos=V GET pos]\n"
  "..\n"
  "[Word pos=w1.pos GET myint, pos]\n"
  "",
  true, true,
  " //  <  < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 6 { 5 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 5 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 21 { 9 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//doing WITH MAX RANGE 5 MONADS
QueryAnswer(false, "doing WITH MAX RANGE 5 MONADS",
  "SELECT ALL OBJECTS IN {1-10}\n"
  "WITH MAX RANGE 5 MONADS\n"
  "WHERE \n"
  "[Word AS w1 pos=V GET pos]\n"
  "..\n"
  "[Word pos=w1.pos GET myint, pos]\n"
  "",
  true, true,
  " //  <  < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 6 { 5 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 5 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 21 { 9 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//doing WITH MAX RANGE 6 MONADS
QueryAnswer(false, "doing WITH MAX RANGE 6 MONADS",
  "SELECT ALL OBJECTS IN {1-10}\n"
  "WITH MAX RANGE 6 MONADS\n"
  "WHERE \n"
  "[Word AS w1 pos=V GET pos]\n"
  "..\n"
  "[Word pos=w1.pos GET myint, pos]\n"
  "",
  true, true,
  " //  <  < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 6 { 5 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 21 { 9 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 5 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 21 { 9 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//doing WITH MAX RANGE 7 MONADS
QueryAnswer(false, "doing WITH MAX RANGE 7 MONADS",
  "SELECT ALL OBJECTS IN {1-10}\n"
  "WITH MAX RANGE 7 MONADS\n"
  "WHERE \n"
  "[Word AS w1 pos=V GET pos]\n"
  "..\n"
  "[Word pos=w1.pos GET myint, pos]\n"
  "",
  true, true,
  " //  <  < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 6 { 5 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 21 { 9 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 5 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 21 { 9 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//doing WITH MAX RANGE 1000 MONADS
QueryAnswer(false, "doing WITH MAX RANGE 1000 MONADS",
  "SELECT ALL OBJECTS IN {1-10}\n"
  "WITH MAX RANGE 1000 MONADS\n"
  "WHERE \n"
  "[Word AS w1 pos=V GET pos]\n"
  "..\n"
  "[Word pos=w1.pos GET myint, pos]\n"
  "",
  true, true,
  " //  <  < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 6 { 5 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 21 { 9 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 5 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 21 { 9 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//doing WITH MAX RANGE FEATURE MONADS FROM [Phrase]
QueryAnswer(false, "doing WITH MAX RANGE FEATURE MONADS FROM [Phrase]",
  "SELECT ALL OBJECTS IN {1-10}\n"
  "WITH MAX RANGE FEATURE MONADS FROM [Phrase]\n"
  "WHERE \n"
  "[Word AS w1 pos=V GET pos]\n"
  "..\n"
  "[Word pos=w1.pos GET myint, pos]\n"
  "",
  true, true,
  " //  <  < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 6 { 5 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//doing WITH MAX RANGE FEATURE MONADS FROM [Clause]
QueryAnswer(false, "doing WITH MAX RANGE FEATURE MONADS FROM [Clause]",
  "SELECT ALL OBJECTS IN {1-10}\n"
  "WITH MAX RANGE FEATURE MONADS FROM [Clause]\n"
  "WHERE \n"
  "[Word AS w1 pos=V GET pos]\n"
  "..\n"
  "[Word pos=w1.pos GET myint, pos]\n"
  "",
  true, true,
  " //  <  < [ word 2 { 2 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 6 { 5 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 6 { 5 } false (pos=\"V\") //  <  > \n"
  " ]\n"
  "[ word 21 { 9 } false (myint=\"2\",pos=\"V\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//
QueryAnswer(false, "",
  "SELECT ALL OBJECTS IN {1-4}\n"
  "WHERE\n"
  "[word as w1 pos = w1.pos]\n"
  "",
  true, true,
  " //  <  < [ word 1 { 1 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 4 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//
QueryAnswer(false, "",
  "SELECT ALL OBJECTS IN {3-7}\n"
  "WHERE\n"
  "[clause part_of(substrate)]\n"
  "",
  true, true,
  " //  <  < [ clause 9 { 4-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//
QueryAnswer(false, "",
  "SELECT ALL OBJECTS \n"
  "IN {2-7}\n"
  "WHERE\n"
  "[clause overlap(substrate)]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause 9 { 4-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//
QueryAnswer(false, "",
  "SELECT ALL OBJECTS IN {1-3, 7-10}\n"
  "WHERE\n"
  "[clause part_of(universe)]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause 9 { 4-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause 17 { 8-10 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//
QueryAnswer(false, "",
  "SELECT ALL OBJECTS IN {1-3,7-9}\n"
  "WHERE\n"
  "[clause overlap(universe)]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause 9 { 4-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause 17 { 8-10 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//
QueryAnswer(false, "",
  "SELECT ALL OBJECTS IN {1-3,8-9}\n"
  "WHERE\n"
  "[clause overlap(substrate)]\n"
  "",
  true, true,
  " //  <  < [ clause 4 { 1-3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ clause 17 { 8-10 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//creating realistic DB schema (2)
QueryAnswer(true, "creating realistic DB schema (2)",
  "CREATE ENUMERATION pos_t = {\n"
  "	def_article, noun, verb, relative_pronoun, preposition, adjective\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE ENUMERATION phrase_type_t = {\n"
  "   NP, VP, PP, AP\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE ENUMERATION phrase_function_t = {\n"
  "   NA, Subj, Objc, Rela, Pred, Loca, PreC, Attr, Adju, Time\n"
  "}\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE MONAD OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[Word\n"
  "   surface: STRING FROM SET;\n"
  "   pos : pos_t;\n"
  "   parent : id_d;\n"
  "   lemma : STRING FROM SET WITH INDEX;\n"
  "   parents : LIST OF id_d;\n"
  "   magic : integer DEFAULT 42;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH MULTIPLE RANGE OBJECTS\n"
  "[Phrase\n"
  "  phrase_type : phrase_type_t;\n"
  "  phrase_function : LIST OF phrase_function_t;\n"
  "  other_monads : set of monads;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[Clause_atom\n"
  "  other_monads : set of monads;\n"
  "]\n"
  "GO\n"
  "\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH MULTIPLE RANGE OBJECTS\n"
  "[Clause\n"
  "  other_monads : set of monads;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH MULTIPLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[Sentence\n"
  "  other_monads : set of monads;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST AND LAST MONADS\n"
  "[Paragraph\n"
  "  other_monads : set of monads;\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  ""),


//creating realistic Word objects
QueryAnswer(false, "creating realistic Word objects",
  "CREATE OBJECTS WITH OBJECT TYPE [Word]\n"
  "CREATE OBJECT FROM MONADS = {1}\n"
  "WITH ID_D = 1\n"
  "[\n"
  "   surface :=\"The\";\n"
  "   pos := def_article;\n"
  "   parent := 10;\n"
  "   lemma := \"the\";\n"
  "   parents := (10,17,20,22,23);\n"
  "   magic := 2;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {2}\n"
  "WITH ID_D = 2\n"
  "[\n"
  "   surface :=\"door,\";\n"
  "   pos := noun;\n"
  "   parent := 10;\n"
  "   lemma := \"door\";\n"
  "   parents := (10,17,20,22,23);\n"
  "   magic := 1;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {3}\n"
  "WITH ID_D = 3\n"
  "[\n"
  "   surface :=\"which\";\n"
  "   pos := relative_pronoun;\n"
  "   parent := 11;\n"
  "   lemma := \"which\";\n"
  "   parents := (11,18,21,22,23);\n"
  "   magic := 3;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {4}\n"
  "WITH ID_D = 4\n"
  "[\n"
  "   surface :=\"opened\";\n"
  "   pos := verb;\n"
  "   parent := 12;\n"
  "   lemma := \"open\";\n"
  "   parents := (12,18,21,22,23);\n"
  "   magic := 7;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {5}\n"
  "WITH ID_D = 5\n"
  "[\n"
  "   surface :=\"towards\";\n"
  "   pos := preposition;\n"
  "   parent := 13;\n"
  "   lemma := \"towards\";\n"
  "   parents := (13,18,21,22,23);\n"
  "   magic := 7;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {6}\n"
  "WITH ID_D = 6\n"
  "[\n"
  "   surface :=\"the\";\n"
  "   pos := def_article;\n"
  "   parent := 14;\n"
  "   lemma := \"the\";\n"
  "   parents := (14,18,21,22,23);\n"
  "   magic := 7;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {7}\n"
  "WITH ID_D = 7\n"
  "[\n"
  "   surface :=\"East,\";\n"
  "   pos := noun;\n"
  "   parent := 14;\n"
  "   lemma := \"east\";\n"
  "   parents := (14,18,21,22,23);\n"
  "   magic := 9;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {8}\n"
  "WITH ID_D = 8\n"
  "[\n"
  "   surface :=\"was\";\n"
  "   pos := verb;\n"
  "   parent := 15;\n"
  "   lemma := \"be\";\n"
  "   parents := (15,19,20,22,23);\n"
  "   magic := 6;\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {9}\n"
  "WITH ID_D = 9\n"
  "[\n"
  "   surface :=\"blue.\";\n"
  "   pos := adjective;\n"
  "   parent := 16;\n"
  "   lemma := \"blue\";\n"
  "   parents := (16,19,20,22,23);\n"
  "   magic := 11;\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 9                      |\n"
  "+------------------------+\n"
  ""),


//creating realistic Phrase objects
QueryAnswer(false, "creating realistic Phrase objects",
  "CREATE OBJECTS WITH OBJECT TYPE [Phrase]\n"
  "CREATE OBJECT FROM MONADS = {1-2}\n"
  "WITH ID_D = 10\n"
  "[\n"
  "  phrase_type := NP;\n"
  "  phrase_function := (Subj);\n"
  "  other_monads := {9};\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {3-3}\n"
  "WITH ID_D = 11\n"
  "[\n"
  "  phrase_type := NP;\n"
  "  phrase_function := (Rela,Subj);\n"
  "  other_monads := {8-8};\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {4-4}\n"
  "WITH ID_D = 12\n"
  "[\n"
  "  phrase_type := VP;\n"
  "  phrase_function := (Pred);\n"
  "  other_monads := {5-7};\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {6-7}\n"
  "WITH ID_D = 14\n"
  "[\n"
  "  phrase_type := NP;\n"
  "  phrase_function := (NA);\n"
  "  other_monads := {6-7};\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {5-7}\n"
  "WITH ID_D = 13\n"
  "[\n"
  "  phrase_type := PP;\n"
  "  phrase_function := (Loca);\n"
  "  other_monads := {3};\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {8-8}\n"
  "WITH ID_D = 15\n"
  "[\n"
  "  phrase_type := VP;\n"
  "  phrase_function := (PreC);\n"
  "  other_monads := {3};\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = {9}\n"
  "WITH ID_D = 16\n"
  "[\n"
  "  phrase_type := AP;\n"
  "  phrase_function := (Attr);\n"
  "  other_monads := {1-2};\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 7                      |\n"
  "+------------------------+\n"
  ""),


//Creating paragraph object with empty other_monads, using CREATE OBJECT.
QueryAnswer(false, "Creating paragraph object with empty other_monads, using CREATE OBJECT.",
  "CREATE OBJECT\n"
  "FROM MONADS = {1-9}\n"
  "WITH ID_D = 100000\n"
  "[Paragraph\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 100000      |\n"
  "+-------------+\n"
  ""),


//Creating paragraph object with non-empty other_monads, using CREATE OBJECT.
QueryAnswer(false, "Creating paragraph object with non-empty other_monads, using CREATE OBJECT.",
  "CREATE OBJECT\n"
  "FROM MONADS = {2-8}\n"
  "WITH ID_D = 100001\n"
  "[Paragraph\n"
  "   other_monads := {1-9};\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 100001      |\n"
  "+-------------+\n"
  ""),


//UPDATING object type Clause
QueryAnswer(false, "UPDATING object type Clause",
  "UPDATE OBJECT TYPE\n"
  "[Clause\n"
  "   REMOVE other_monads;\n"
  "   ADD surface: STRING FROM SET;\n"
  "   ADD pos : pos_t;\n"
  "   ADD parent : id_d;\n"
  "   ADD lemma : STRING FROM SET WITH INDEX;\n"
  "   ADD parents : LIST OF id_d;\n"
  "   ADD other_monads2 : SET OF MONADS;\n"
  "   ADD magic : integer DEFAULT 42;\n"
  "]\n"
  "",
  true, true,
  ""),


//SELECT ALL OBJECTS from paragraph in 1-9 (1)
QueryAnswer(false, "SELECT ALL OBJECTS from paragraph in 1-9 (1)",
  "SELECT ALL OBJECTS\n"
  "IN {1-9}\n"
  "WHERE\n"
  "[Paragraph\n"
  "   GET other_monads\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ paragraph 100000 { 1-9 } false (other_monads=\" {  } \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ paragraph 100001 { 2-8 } false (other_monads=\" { 1-9 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//UDPATE OBJECT paragraph
QueryAnswer(false, "UDPATE OBJECT paragraph",
  "UPDATE OBJECT BY MONADS = {1-3}\n"
  "[Paragraph\n"
  "   other_monads := {7-9};\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 100000      |\n"
  "| 100001      |\n"
  "+-------------+\n"
  ""),


//SELECT ALL OBJECTS from paragraph in 1-9 (2)
QueryAnswer(false, "SELECT ALL OBJECTS from paragraph in 1-9 (2)",
  "SELECT ALL OBJECTS\n"
  "IN {1-9}\n"
  "WHERE\n"
  "[Paragraph\n"
  "   GET other_monads\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ paragraph 100000 { 1-9 } false (other_monads=\" { 7-9 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ paragraph 100001 { 2-8 } false (other_monads=\" { 7-9 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//UDPATE OBJECT paragraph (3)
QueryAnswer(false, "UDPATE OBJECT paragraph (3)",
  "UPDATE OBJECTS BY ID_D = 100000,100001\n"
  "[Paragraph\n"
  "   other_monads := {5-7};\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 100000      |\n"
  "| 100001      |\n"
  "+-------------+\n"
  ""),


//SELECT ALL OBJECTS from paragraph in 1-9 (2)
QueryAnswer(false, "SELECT ALL OBJECTS from paragraph in 1-9 (2)",
  "SELECT ALL OBJECTS\n"
  "IN {1-9}\n"
  "WHERE\n"
  "[Paragraph\n"
  "   GET other_monads\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ paragraph 100000 { 1-9 } false (other_monads=\" { 5-7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ paragraph 100001 { 2-8 } false (other_monads=\" { 5-7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//SELECT ALL OBJECTS from phrase in 1-6 (1)
QueryAnswer(false, "SELECT ALL OBJECTS from phrase in 1-6 (1)",
  "SELECT ALL OBJECTS IN {1-6} WHERE\n"
  "[Phrase\n"
  "  GET phrase_function, phrase_type, other_monads\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false (phrase_function=\" Subj \",phrase_type=\"NP\",other_monads=\" { 9 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false (phrase_function=\" Rela Subj \",phrase_type=\"NP\",other_monads=\" { 8 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false (phrase_function=\" Pred \",phrase_type=\"VP\",other_monads=\" { 5-7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//Updating Phrase object type in monads {1-3}
QueryAnswer(false, "Updating Phrase object type in monads {1-3}",
  "UPDATE OBJECTS BY MONADS = {1-3}\n"
  "[Phrase\n"
  "  phrase_function := (Adju,Time);\n"
  "  phrase_type := PP;\n"
  "  other_monads := {1-3};\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 10          |\n"
  "| 11          |\n"
  "+-------------+\n"
  ""),


//SELECT ALL OBJECTS from phrase in 1-6 (2)
QueryAnswer(false, "SELECT ALL OBJECTS from phrase in 1-6 (2)",
  "SELECT ALL OBJECTS IN {1-6} WHERE\n"
  "[Phrase\n"
  "  GET phrase_function, phrase_type, other_monads\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false (phrase_function=\" Adju Time \",phrase_type=\"PP\",other_monads=\" { 1-3 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false (phrase_function=\" Adju Time \",phrase_type=\"PP\",other_monads=\" { 1-3 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false (phrase_function=\" Pred \",phrase_type=\"VP\",other_monads=\" { 5-7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//Updating phrase objects by id_d
QueryAnswer(false, "Updating phrase objects by id_d",
  "UPDATE OBJECTS BY ID_D = 10,12\n"
  "[Phrase\n"
  "  phrase_function := (Time,Adju);\n"
  "  phrase_type := AP;\n"
  "  other_monads := {3-5, 8-9};\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 10          |\n"
  "| 12          |\n"
  "+-------------+\n"
  ""),


//Updating phrase objects by id_d (2)
QueryAnswer(false, "Updating phrase objects by id_d (2)",
  "UPDATE OBJECTS BY ID_D = 16\n"
  "[Phrase\n"
  "  phrase_type := NP;\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 16          |\n"
  "+-------------+\n"
  ""),


//SELECT ALL OBJECTS from phrase in 1-6 (2)
QueryAnswer(false, "SELECT ALL OBJECTS from phrase in 1-6 (2)",
  "SELECT ALL OBJECTS IN {1-6} WHERE\n"
  "[Phrase\n"
  "  GET phrase_function, phrase_type, other_monads\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ phrase 10 { 1-2 } false (phrase_function=\" Time Adju \",phrase_type=\"AP\",other_monads=\" { 3-5, 8-9 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 11 { 3 } false (phrase_function=\" Adju Time \",phrase_type=\"PP\",other_monads=\" { 1-3 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 4 } false (phrase_function=\" Time Adju \",phrase_type=\"AP\",other_monads=\" { 3-5, 8-9 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//selecting features from object type phrase
QueryAnswer(false, "selecting features from object type phrase",
  "SELECT FEATURES FROM OBJECT TYPE [Phrase] GO\n"
  "",
  true, true,
  "+-----------------------+---------------------------+------------------+-----------------+\n"
  "| feature name : string | type : string             | default : string | computed : bool |\n"
  "+-----------------------+---------------------------+------------------+-----------------+\n"
  "| other_monads          | set of monads             | ' {  } '         | false           |\n"
  "| phrase_function       | list of phrase_function_t | ''               | false           |\n"
  "| phrase_type           | phrase_type_t             | 'NP'             | false           |\n"
  "| self                  | id_d                      | '0'              | true            |\n"
  "+-----------------------+---------------------------+------------------+-----------------+\n"
  ""),


//getting features from object type phrase
QueryAnswer(false, "getting features from object type phrase",
  "GET FEATURES self, other_monads, phrase_function, phrase_type\n"
  "FROM OBJECTS WITH ID_DS = 10,11,12\n"
  "[Phrase]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+-------------+------------------------------+---------------------------------------------------+-----------------------------------+\n"
  "| id_d : id_d | self : id_d | other_monads : set_of_monads | phrase_function : list_of_enum(phrase_function_t) | phrase_type : enum(phrase_type_t) |\n"
  "+-------------+-------------+------------------------------+---------------------------------------------------+-----------------------------------+\n"
  "| 10          | 10          |  { 3-5, 8-9 }                |  Time Adju                                        | AP                                |\n"
  "| 11          | 11          |  { 1-3 }                     |  Adju Time                                        | PP                                |\n"
  "| 12          | 12          |  { 3-5, 8-9 }                |  Time Adju                                        | AP                                |\n"
  "+-------------+-------------+------------------------------+---------------------------------------------------+-----------------------------------+\n"
  ""),


//getting the compiler to fail on part_of(phrase_function, substrate)
QueryAnswer(false, "getting the compiler to fail on part_of(phrase_function, substrate)",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase part_of(phrase_function, substrate)\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Typechecking failed with compiler error\n"
  "Although the feature phrase_function exists\n"
  "on the object type of the object block with a monad set relation clause,\n"
  "it is not of the type SET OF MONADS and therefore cannot be used in the\n"
  "monad set relation clause.\n"
  ""),


//getting the compiler to fail on part_of(does_not_exist, substrate)
QueryAnswer(false, "getting the compiler to fail on part_of(does_not_exist, substrate)",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase part_of(does_not_exist, substrate)\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "The feature does_not_exist does not exist\n"
  "on the object type of the object block with a monad set relation clause.\n"
  ""),


//UPDATING object type Clause
QueryAnswer(false, "UPDATING object type Clause",
  "UPDATE OBJECT TYPE\n"
  "[Clause\n"
  "   ADD other_monads : SET OF MONADS;\n"
  "]\n"
  "",
  true, true,
  ""),


//Creating paragraph object with non-empty other_monads, using CREATE OBJECT.
QueryAnswer(false, "Creating paragraph object with non-empty other_monads, using CREATE OBJECT.",
  "CREATE OBJECT\n"
  "FROM MONADS = {1-9}\n"
  "WITH ID_D = 11000000\n"
  "[Clause\n"
  "   other_monads := {3-7,10-12};\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 11000000    |\n"
  "+-------------+\n"
  ""),


//Creating paragraph object with non-empty other_monads, using CREATE OBJECT.
QueryAnswer(false, "Creating paragraph object with non-empty other_monads, using CREATE OBJECT.",
  "CREATE OBJECT\n"
  "FROM MONADS = {5-7}\n"
  "WITH ID_D = 11000001\n"
  "[Clause\n"
  "   other_monads := {1-4,8-12};\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 11000001    |\n"
  "+-------------+\n"
  ""),


//select all objects with part_of(other_monads,substrate)
QueryAnswer(false, "select all objects with part_of(other_monads,substrate)",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase part_of(other_monads, substrate)\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 11 { 1-3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 10 { 3-5, 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 3-5, 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//select all objects with overlap(other_monads,substrate)
QueryAnswer(false, "select all objects with overlap(other_monads,substrate)",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase overlap(other_monads, substrate)\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 11 { 1-3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 10 { 3-5, 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 3-5, 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 3 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 10 { 3-5, 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 3-5, 8-9 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//selecting phrase with overlap of clause substrate, and get tokens inside
QueryAnswer(false, "selecting phrase with overlap of clause substrate, and get tokens inside",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase overlap(other_monads, substrate)\n"
  "      [Word GET surface]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 11 { 1-3 } false  //  <  < [ word 1 { 1 } false (surface=\"The\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false (surface=\"door,\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 3 { 3 } false (surface=\"which\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 16 { 1-2 } false  //  <  < [ word 1 { 1 } false (surface=\"The\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false (surface=\"door,\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 10 { 3-5, 8-9 } false  //  <  < [ word 3 { 3 } false (surface=\"which\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false (surface=\"opened\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false (surface=\"towards\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false (surface=\"was\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false (surface=\"blue.\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 3-5, 8-9 } false  //  <  < [ word 3 { 3 } false (surface=\"which\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false (surface=\"opened\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false (surface=\"towards\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false (surface=\"was\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false (surface=\"blue.\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 13 { 3 } false  //  <  < [ word 3 { 3 } false (surface=\"which\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 15 { 3 } false  //  <  < [ word 3 { 3 } false (surface=\"which\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false (surface=\"the\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false (surface=\"East,\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 10 { 3-5, 8-9 } false  //  <  < [ word 3 { 3 } false (surface=\"which\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false (surface=\"opened\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false (surface=\"towards\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false (surface=\"was\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false (surface=\"blue.\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 12 { 3-5, 8-9 } false  //  <  < [ word 3 { 3 } false (surface=\"which\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 4 { 4 } false (surface=\"opened\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 5 { 5 } false (surface=\"towards\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 8 { 8 } false (surface=\"was\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 9 { 9 } false (surface=\"blue.\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false (surface=\"the\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false (surface=\"East,\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//selecting phrase with overlap of clause substrate and a feature-constraints with an index
QueryAnswer(false, "selecting phrase with overlap of clause substrate and a feature-constraints with an index",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase overlap(other_monads, substrate) phrase_type=NP\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 16 { 1-2 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 14 { 6-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get tokens inside
QueryAnswer(false, "selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get tokens inside",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase overlap(other_monads, substrate) phrase_type=NP\n"
  "      [Word GET surface]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 16 { 1-2 } false  //  <  < [ word 1 { 1 } false (surface=\"The\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 2 { 2 } false (surface=\"door,\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false (surface=\"the\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false (surface=\"East,\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false (surface=\"the\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ word 7 { 7 } false (surface=\"East,\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get def_article tokens inside
QueryAnswer(false, "selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get def_article tokens inside",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase overlap(other_monads, substrate) phrase_type=NP\n"
  "      [Word pos=def_article GET surface]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 16 { 1-2 } false  //  <  < [ word 1 { 1 } false (surface=\"The\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false (surface=\"the\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false (surface=\"the\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get two def_article/noun tokens inside
QueryAnswer(false, "selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get two def_article/noun tokens inside",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase overlap(other_monads, substrate) phrase_type=NP\n"
  "      [Word pos=def_article GET surface]\n"
  "      [Word pos=noun GET surface]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 16 { 1-2 } false  //  <  < [ word 1 { 1 } false (surface=\"The\") //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false (surface=\"door,\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false (surface=\"the\") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (surface=\"East,\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 14 { 6-7 } false  //  <  < [ word 6 { 6 } false (surface=\"the\") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (surface=\"East,\") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get two def_article/noun tokens inside
QueryAnswer(false, "selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get two def_article/noun tokens inside",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase overlap(other_monads, substrate) phrase_type=NP GET monads\n"
  "     [Word pos=def_article GET surface, monads]\n"
  "     [Word pos=noun GET surface, monads]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 16 { 1-2 } false (monads=\" { 9 } \") //  <  < [ word 1 { 1 } false (surface=\"The\",monads=\" { 1 } \") //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false (surface=\"door,\",monads=\" { 2 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false (monads=\" { 6-7 } \") //  <  < [ word 6 { 6 } false (surface=\"the\",monads=\" { 6 } \") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (surface=\"East,\",monads=\" { 7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 14 { 6-7 } false (monads=\" { 6-7 } \") //  <  < [ word 6 { 6 } false (surface=\"the\",monads=\" { 6 } \") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (surface=\"East,\",monads=\" { 7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get two def_article/noun tokens inside
QueryAnswer(false, "selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get two def_article/noun tokens inside",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase part_of(other_monads, substrate) phrase_type=NP GET monads\n"
  "     [Word pos=def_article GET surface, monads]\n"
  "     [Word pos=noun GET surface, monads]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 16 { 1-2 } false (monads=\" { 9 } \") //  <  < [ word 1 { 1 } false (surface=\"The\",monads=\" { 1 } \") //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false (surface=\"door,\",monads=\" { 2 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false (monads=\" { 6-7 } \") //  <  < [ word 6 { 6 } false (surface=\"the\",monads=\" { 6 } \") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (surface=\"East,\",monads=\" { 7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 14 { 6-7 } false (monads=\" { 6-7 } \") //  <  < [ word 6 { 6 } false (surface=\"the\",monads=\" { 6 } \") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (surface=\"East,\",monads=\" { 7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get two def_article/noun tokens inside
QueryAnswer(false, "selecting phrase with overlap of clause substrate and a feature-constraints with an index, and get two def_article/noun tokens inside",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase overlap(other_monads, substrate) phrase_type=NP GET monads, other_monads\n"
  "     [Word pos=def_article GET surface, monads]\n"
  "     [Word pos=noun GET surface, monads]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 16 { 1-2 } false (monads=\" { 9 } \",other_monads=\" { 1-2 } \") //  <  < [ word 1 { 1 } false (surface=\"The\",monads=\" { 1 } \") //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false (surface=\"door,\",monads=\" { 2 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false (monads=\" { 6-7 } \",other_monads=\" { 6-7 } \") //  <  < [ word 6 { 6 } false (surface=\"the\",monads=\" { 6 } \") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (surface=\"East,\",monads=\" { 7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 14 { 6-7 } false (monads=\" { 6-7 } \",other_monads=\" { 6-7 } \") //  <  < [ word 6 { 6 } false (surface=\"the\",monads=\" { 6 } \") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (surface=\"East,\",monads=\" { 7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//selecting phrase with part_of relation between clause and substrate and a feature-constraints with an index, and get two def_article/noun tokens inside, getting monads and other_monads
QueryAnswer(false, "selecting phrase with part_of relation between clause and substrate and a feature-constraints with an index, and get two def_article/noun tokens inside, getting monads and other_monads",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Clause\n"
  "   [phrase part_of(other_monads, substrate) phrase_type=NP GET monads, other_monads\n"
  "     [Word pos=def_article GET surface, monads]\n"
  "     [Word pos=noun GET surface, monads]\n"
  "   ]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ clause 11000000 { 1-9 } false  //  <  < [ phrase 16 { 1-2 } false (monads=\" { 9 } \",other_monads=\" { 1-2 } \") //  <  < [ word 1 { 1 } false (surface=\"The\",monads=\" { 1 } \") //  <  > \n"
  " ]\n"
  "[ word 2 { 2 } false (surface=\"door,\",monads=\" { 2 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ phrase 14 { 6-7 } false (monads=\" { 6-7 } \",other_monads=\" { 6-7 } \") //  <  < [ word 6 { 6 } false (surface=\"the\",monads=\" { 6 } \") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (surface=\"East,\",monads=\" { 7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ clause 11000001 { 5-7 } false  //  <  < [ phrase 14 { 6-7 } false (monads=\" { 6-7 } \",other_monads=\" { 6-7 } \") //  <  < [ word 6 { 6 } false (surface=\"the\",monads=\" { 6 } \") //  <  > \n"
  " ]\n"
  "[ word 7 { 7 } false (surface=\"East,\",monads=\" { 7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//GET OBJECTS HAVING MOANDS IN from single monad objects 'Word' with GET monads
QueryAnswer(false, "GET OBJECTS HAVING MOANDS IN from single monad objects 'Word' with GET monads",
  "GET OBJECTS HAVING MONADS IN {1-5}\n"
  "[Word GET monads]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ Word 1 { 1 } false (monads=\" { 1 } \") //  <  > \n"
  " ]\n"
  "[ Word 2 { 2 } false (monads=\" { 2 } \") //  <  > \n"
  " ]\n"
  "[ Word 3 { 3 } false (monads=\" { 3 } \") //  <  > \n"
  " ]\n"
  "[ Word 4 { 4 } false (monads=\" { 4 } \") //  <  > \n"
  " ]\n"
  "[ Word 5 { 5 } false (monads=\" { 5 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//GET OBJECTS HAVING MOANDS IN from single range objects 'Paragraph' with GET monads
QueryAnswer(false, "GET OBJECTS HAVING MOANDS IN from single range objects 'Paragraph' with GET monads",
  "GET OBJECTS HAVING MONADS IN {1-5}\n"
  "[Paragraph GET monads]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ Paragraph 100000 { 1-9 } false (monads=\" { 1-9 } \") //  <  > \n"
  " ]\n"
  "[ Paragraph 100001 { 2-8 } false (monads=\" { 2-8 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//GET OBJECTS HAVING MOANDS IN from multiple range objects 'Clause' with GET monads
QueryAnswer(false, "GET OBJECTS HAVING MOANDS IN from multiple range objects 'Clause' with GET monads",
  "GET OBJECTS HAVING MONADS IN {1-5}\n"
  "[Clause GET monads]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ Clause 11000000 { 1-9 } false (monads=\" { 1-9 } \") //  <  > \n"
  " ]\n"
  "[ Clause 11000001 { 5-7 } false (monads=\" { 5-7 } \") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//getting features from word object with monad set retrieval 1
QueryAnswer(false, "getting features from word object with monad set retrieval 1",
  "GET FEATURES surface, pos, monads, parent, magic\n"
  "FROM OBJECTS WITH ID_DS = 2,5\n"
  "[Word]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+------------------+-------------------+------------------------+---------------+-----------------+\n"
  "| id_d : id_d | surface : string | pos : enum(pos_t) | monads : set_of_monads | parent : id_d | magic : integer |\n"
  "+-------------+------------------+-------------------+------------------------+---------------+-----------------+\n"
  "| 2           | door,            | noun              |  { 2 }                 | 10            | 1               |\n"
  "| 5           | towards          | preposition       |  { 5 }                 | 13            | 7               |\n"
  "+-------------+------------------+-------------------+------------------------+---------------+-----------------+\n"
  ""),


//getting features from paragraph object with monad set retrieval 1
QueryAnswer(false, "getting features from paragraph object with monad set retrieval 1",
  "GET FEATURES monads, other_monads, self\n"
  "FROM OBJECTS WITH ID_DS = 100001, 100000\n"
  "[paragraph]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+------------------------+------------------------------+-------------+\n"
  "| id_d : id_d | monads : set_of_monads | other_monads : set_of_monads | self : id_d |\n"
  "+-------------+------------------------+------------------------------+-------------+\n"
  "| 100000      |  { 1-9 }               |  { 5-7 }                     | 100000      |\n"
  "| 100001      |  { 2-8 }               |  { 5-7 }                     | 100001      |\n"
  "+-------------+------------------------+------------------------------+-------------+\n"
  ""),


//getting features from object type phrase
QueryAnswer(false, "getting features from object type phrase",
  "GET FEATURES monads, self, other_monads, phrase_function, phrase_type\n"
  "FROM OBJECTS WITH ID_DS = 10,11,12\n"
  "[Phrase]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+------------------------+-------------+------------------------------+---------------------------------------------------+-----------------------------------+\n"
  "| id_d : id_d | monads : set_of_monads | self : id_d | other_monads : set_of_monads | phrase_function : list_of_enum(phrase_function_t) | phrase_type : enum(phrase_type_t) |\n"
  "+-------------+------------------------+-------------+------------------------------+---------------------------------------------------+-----------------------------------+\n"
  "| 10          |  { 1-2 }               | 10          |  { 3-5, 8-9 }                |  Time Adju                                        | AP                                |\n"
  "| 11          |  { 3 }                 | 11          |  { 1-3 }                     |  Adju Time                                        | PP                                |\n"
  "| 12          |  { 4 }                 | 12          |  { 3-5, 8-9 }                |  Time Adju                                        | AP                                |\n"
  "+-------------+------------------------+-------------+------------------------------+---------------------------------------------------+-----------------------------------+\n"
  ""),


//creating object type describer with SET OF MONADS feature
QueryAnswer(false, "creating object type describer with SET OF MONADS feature",
  "CREATE OBJECT TYPE\n"
  "[Describer\n"
  "    described_som : SET OF MONADS;\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  ""),


//creating objects in object type Describer
QueryAnswer(false, "creating objects in object type Describer",
  "CREATE OBJECTS WITH OBJECT TYPE [Describer]\n"
  "CREATE OBJECT FROM MONADS = { 1-396 }\n"
  "WITH ID_D = 240000\n"
  "[	\n"
  "described_som := {40201};	\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = { 397-550 }\n"
  "WITH ID_D = 240001\n"
  "[	\n"
  "described_som := {40201-40400};	\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = { 551-804 }\n"
  "WITH ID_D = 240002\n"
  "[	\n"
  "described_som := {40201-40400};	\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = { 805-998 }\n"
  "WITH ID_D = 240003\n"
  "[	\n"
  "described_som := {40202};	\n"
  "]\n"
  "CREATE OBJECT FROM MONADS = { 999-1038 }\n"
  "WITH ID_D = 240004\n"
  "[	\n"
  "described_som := {40202-44400};	\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 5                      |\n"
  "+------------------------+\n"
  ""),


//selecting non-monads monad set with overlap in topographic query #1
QueryAnswer(false, "selecting non-monads monad set with overlap in topographic query #1",
  "SELECT ALL OBJECTS \n"
  "IN {40201} \n"
  "RETURNING FLAT SHEAF \n"
  "WHERE \n"
  "[Describer overlap(described_som, substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240000 { 40201 } false (self=\"240000\") //  <  > \n"
  " ]\n"
  "[ describer 240001 { 40201-40400 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ describer 240002 { 40201-40400 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with overlap in topographic query #2
QueryAnswer(false, "selecting non-monads monad set with overlap in topographic query #2",
  "SELECT ALL OBJECTS \n"
  "IN {40202} \n"
  "RETURNING FLAT SHEAF \n"
  "WHERE \n"
  "[Describer overlap(described_som, substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240001 { 40201-40400 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ describer 240002 { 40201-40400 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ describer 240003 { 40202 } false (self=\"240003\") //  <  > \n"
  " ]\n"
  "[ describer 240004 { 40202-44400 } false (self=\"240004\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with overlap in topographic query #3
QueryAnswer(false, "selecting non-monads monad set with overlap in topographic query #3",
  "SELECT ALL OBJECTS \n"
  "IN {40202-40203} \n"
  "RETURNING FLAT SHEAF \n"
  "WHERE \n"
  "[Describer overlap(described_som, substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240001 { 40201-40400 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ describer 240002 { 40201-40400 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ describer 240003 { 40202 } false (self=\"240003\") //  <  > \n"
  " ]\n"
  "[ describer 240004 { 40202-44400 } false (self=\"240004\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with overlap in topographic query #4
QueryAnswer(false, "selecting non-monads monad set with overlap in topographic query #4",
  "SELECT ALL OBJECTS \n"
  "IN {40201-50000} \n"
  "RETURNING FLAT SHEAF \n"
  "WHERE \n"
  "[Describer overlap(described_som, substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240000 { 40201 } false (self=\"240000\") //  <  > \n"
  " ]\n"
  "[ describer 240001 { 40201-40400 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ describer 240002 { 40201-40400 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ describer 240003 { 40202 } false (self=\"240003\") //  <  > \n"
  " ]\n"
  "[ describer 240004 { 40202-44400 } false (self=\"240004\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with overlap in topographic query #5
QueryAnswer(false, "selecting non-monads monad set with overlap in topographic query #5",
  "SELECT ALL OBJECTS \n"
  "IN {40200-40450} \n"
  "RETURNING FLAT SHEAF \n"
  "WHERE \n"
  "[Describer overlap(described_som, substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240000 { 40201 } false (self=\"240000\") //  <  > \n"
  " ]\n"
  "[ describer 240001 { 40201-40400 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ describer 240002 { 40201-40400 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ describer 240003 { 40202 } false (self=\"240003\") //  <  > \n"
  " ]\n"
  "[ describer 240004 { 40202-44400 } false (self=\"240004\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with overlap in topographic query #6
QueryAnswer(false, "selecting non-monads monad set with overlap in topographic query #6",
  "SELECT ALL OBJECTS \n"
  "IN {1-40}\n"
  "RETURNING FLAT SHEAF\n"
  "WHERE \n"
  "[Describer overlap(described_som, substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // \n"
  ""),


//selecting non-monads monad set with overlap in topographic query #7
QueryAnswer(false, "selecting non-monads monad set with overlap in topographic query #7",
  "SELECT ALL OBJECTS \n"
  "IN {1-50000000}\n"
  "RETURNING FLAT SHEAF\n"
  "WHERE \n"
  "[Describer overlap(described_som, substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240000 { 40201 } false (self=\"240000\") //  <  > \n"
  " ]\n"
  "[ describer 240001 { 40201-40400 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ describer 240002 { 40201-40400 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ describer 240003 { 40202 } false (self=\"240003\") //  <  > \n"
  " ]\n"
  "[ describer 240004 { 40202-44400 } false (self=\"240004\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with overlap in topographic query #8
QueryAnswer(false, "selecting non-monads monad set with overlap in topographic query #8",
  "SELECT ALL OBJECTS \n"
  "IN {2-40200}\n"
  "RETURNING FLAT SHEAF\n"
  "WHERE \n"
  "[Describer overlap(described_som, substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // \n"
  ""),


//get objects having monads in MONADS feature with explicit MONADS feature
QueryAnswer(false, "get objects having monads in MONADS feature with explicit MONADS feature",
  "GET OBJECTS HAVING MONADS IN\n"
  "{ 2-997 }\n"
  "USING MONAD FEATURE MONADS\n"
  "[Describer GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ Describer 240000 { 1-396 } false (self=\"240000\") //  <  > \n"
  " ]\n"
  "[ Describer 240001 { 397-550 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ Describer 240002 { 551-804 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ Describer 240003 { 805-998 } false (self=\"240003\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//get objects having monads in MONADS feature non-MONADS feature #1
QueryAnswer(false, "get objects having monads in MONADS feature non-MONADS feature #1",
  "GET OBJECTS HAVING MONADS IN\n"
  "{ 40000-44399 }\n"
  "USING MONAD FEATURE described_SOM\n"
  "[Describer GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ Describer 240000 { 40201 } false (self=\"240000\") //  <  > \n"
  " ]\n"
  "[ Describer 240001 { 40201-40400 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ Describer 240002 { 40201-40400 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ Describer 240003 { 40202 } false (self=\"240003\") //  <  > \n"
  " ]\n"
  "[ Describer 240004 { 40202-44400 } false (self=\"240004\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//get objects having monads in MONADS feature non-MONADS feature #2
QueryAnswer(false, "get objects having monads in MONADS feature non-MONADS feature #2",
  "GET OBJECTS HAVING MONADS IN\n"
  " { 2-40201, 40203-40500 }\n"
  "USING MONAD FEATURE described_SOM\n"
  "[Describer GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ Describer 240000 { 40201 } false (self=\"240000\") //  <  > \n"
  " ]\n"
  "[ Describer 240001 { 40201-40400 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ Describer 240002 { 40201-40400 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ Describer 240004 { 40202-44400 } false (self=\"240004\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//get objects having monads in MONADS feature non-MONADS feature #3
QueryAnswer(false, "get objects having monads in MONADS feature non-MONADS feature #3",
  "GET OBJECTS HAVING MONADS IN\n"
  " { 2-40201, 40203-50000 }\n"
  "USING MONAD FEATURE described_SOM\n"
  "[Describer GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ Describer 240000 { 40201 } false (self=\"240000\") //  <  > \n"
  " ]\n"
  "[ Describer 240001 { 40201-40400 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ Describer 240002 { 40201-40400 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ Describer 240004 { 40202-44400 } false (self=\"240004\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//get objects having monads in MONADS feature non-MONADS feature #4
QueryAnswer(false, "get objects having monads in MONADS feature non-MONADS feature #4",
  "GET OBJECTS HAVING MONADS IN\n"
  " { 2-40201, 40203-50000 }\n"
  "USING MONAD FEATURE described_SOM\n"
  "[Describer GET ALL]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ Describer 240000 { 40201 } false (described_som=\" { 40201 } \",self=\"240000\") //  <  > \n"
  " ]\n"
  "[ Describer 240001 { 40201-40400 } false (described_som=\" { 40201-40400 } \",self=\"240001\") //  <  > \n"
  " ]\n"
  "[ Describer 240002 { 40201-40400 } false (described_som=\" { 40201-40400 } \",self=\"240002\") //  <  > \n"
  " ]\n"
  "[ Describer 240004 { 40202-44400 } false (described_som=\" { 40202-44400 } \",self=\"240004\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//get objects having monads in MONADS feature non-MONADS feature #5
QueryAnswer(false, "get objects having monads in MONADS feature non-MONADS feature #5",
  "GET OBJECTS HAVING MONADS IN\n"
  " { 2-40201, 40203-50000 }\n"
  "USING MONAD FEATURE described_SOM\n"
  "[Describer]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ Describer 240000 { 40201 } false  //  <  > \n"
  " ]\n"
  "[ Describer 240001 { 40201-40400 } false  //  <  > \n"
  " ]\n"
  "[ Describer 240002 { 40201-40400 } false  //  <  > \n"
  " ]\n"
  "[ Describer 240004 { 40202-44400 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with starts_in in topographic query #1
QueryAnswer(false, "selecting non-monads monad set with starts_in in topographic query #1",
  "SELECT ALL OBJECTS \n"
  "IN {40202, 44000}\n"
  "RETURNING FLAT SHEAF\n"
  "WHERE \n"
  "[Describer starts_in(described_som, substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240003 { 40202 } false (self=\"240003\") //  <  > \n"
  " ]\n"
  "[ describer 240004 { 40202-44400 } false (self=\"240004\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with starts_in in topographic query #2
QueryAnswer(false, "selecting non-monads monad set with starts_in in topographic query #2",
  "SELECT ALL OBJECTS \n"
  "IN {44000}\n"
  "RETURNING FLAT SHEAF\n"
  "WHERE \n"
  "[Describer starts_in(described_som, substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // \n"
  ""),


//selecting non-monads monad set with starts_in in topographic query #3
QueryAnswer(false, "selecting non-monads monad set with starts_in in topographic query #3",
  "SELECT ALL OBJECTS \n"
  "IN {396,397,804}\n"
  "RETURNING FLAT SHEAF\n"
  "WHERE \n"
  "[Describer starts_in(substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240001 { 397-550 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with starts_in in topographic query #4
QueryAnswer(false, "selecting non-monads monad set with starts_in in topographic query #4",
  "SELECT ALL OBJECTS \n"
  "IN {396-800}\n"
  "RETURNING FLAT SHEAF\n"
  "WHERE \n"
  "[Describer starts_in(substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240001 { 397-550 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ describer 240002 { 551-804 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with starts_in in topographic query #5
QueryAnswer(false, "selecting non-monads monad set with starts_in in topographic query #5",
  "SELECT ALL OBJECTS \n"
  "IN {396-806}\n"
  "RETURNING FLAT SHEAF\n"
  "WHERE \n"
  "[Describer starts_in(substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240001 { 397-550 } false (self=\"240001\") //  <  > \n"
  " ]\n"
  "[ describer 240002 { 551-804 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ describer 240003 { 805-998 } false (self=\"240003\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//selecting non-monads monad set with starts_in in topographic query #6
QueryAnswer(false, "selecting non-monads monad set with starts_in in topographic query #6",
  "SELECT ALL OBJECTS \n"
  "IN {550-998}\n"
  "RETURNING FLAT SHEAF\n"
  "WHERE \n"
  "[Describer starts_in(substrate) GET self]\n"
  "GO\n"
  "",
  true, true,
  " // <  < [ describer 240002 { 551-804 } false (self=\"240002\") //  <  > \n"
  " ]\n"
  "[ describer 240003 { 805-998 } false (self=\"240003\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//create object type with extremely long name...
QueryAnswer(false, "create object type with extremely long name...",
  "CREATE OBJECT TYPE\n"
  "[otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah\n"
  "  MYINTEGER : INTEGER;\n"
  "  MYID_D : id_d;\n"
  "  MYASCII : ASCII;\n"
  "  MYSTRING : STRING;\n"
  "  MYSTRINGFROMSET : STRING FROM SET;\n"
  "  MYSOM : SET OF MONADS;\n"
  "  MYENUM : phrase_type_t;\n"
  "  MYLISTOFINTEGER : LIST OF INTEGER;\n"
  "  MYLISTOFID_D : LIST OF ID_D;\n"
  "  MYLISTOFENUM : LIST OF Phrase_type_t;\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  ""),


//creating one object (1) inside the long-named object type
QueryAnswer(false, "creating one object (1) inside the long-named object type",
  "CREATE OBJECT\n"
  "FROM MONADS = {1}\n"
  "[otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah\n"
  "  myinteger := 100;\n"
  "  myid_d := nil;\n"
  "  myascii := \"Blah\";\n"
  "  mystring := \"Blah blah\";\n"
  "  mystringfromset := \"BLAH\";\n"
  "  mysom := { 1-10 };\n"
  "  myenum := NP;\n"
  "  mylistofinteger := (1,2,3);\n"
  "  mylistofid_d := (1,2,3);\n"
  "  mylistofenum := (VP,NP);\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 11000002    |\n"
  "+-------------+\n"
  ""),


//creating one object (2) inside the long-named object type
QueryAnswer(false, "creating one object (2) inside the long-named object type",
  "CREATE OBJECT\n"
  "FROM MONADS = {2}\n"
  "[otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah\n"
  "  myinteger := 101;\n"
  "  myid_d := nil;\n"
  "  myascii := \"Blah2\";\n"
  "  mystring := \"Blah2 blah2\";\n"
  "  mystringfromset := \"BLAH2\";\n"
  "  mysom := { 1-12 };\n"
  "  myenum := VP;\n"
  "  mylistofinteger := (1,2,3,4000000);\n"
  "  mylistofid_d := (1,2,3,3000000);\n"
  "  mylistofenum := (NP,VP);\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+-------------+\n"
  "| id_d : id_d |\n"
  "+-------------+\n"
  "| 11000003    |\n"
  "+-------------+\n"
  ""),


//creating more than one object inside the long-named object type
QueryAnswer(false, "creating more than one object inside the long-named object type",
  "CREATE OBJECTS WITH OBJECT TYPE [otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {3}\n"
  "[\n"
  "  myinteger := 100;\n"
  "  myid_d := nil;\n"
  "  myascii := \"Blah\";\n"
  "  mystring := \"Blah blah\";\n"
  "  mystringfromset := \"BLAH\";\n"
  "  mysom := { 1-10 };\n"
  "  myenum := NP;\n"
  "  mylistofinteger := (1,2,3);\n"
  "  mylistofid_d := (1,2,3);\n"
  "  mylistofenum := ();\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS = {4}\n"
  "[\n"
  "  myinteger := 100;\n"
  "  myid_d := nil;\n"
  "  myascii := \"Blah\";\n"
  "  mystring := \"Blah blah\";\n"
  "  mystringfromset := \"BLAH\";\n"
  "  mysom := { 4 };\n"
  "  myenum := NP;\n"
  "  mylistofinteger := (1,2,3);\n"
  "  mylistofid_d := ();\n"
  "  mylistofenum := (VP);\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 2                      |\n"
  "+------------------------+\n"
  ""),


//UPDATE OBJECT TYPE on very long Object Type Name (1)
QueryAnswer(false, "UPDATE OBJECT TYPE on very long Object Type Name (1)",
  "UPDATE TYPE [otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah\n"
  "	ADD willbeadded : integer;\n"
  "]\n"
  "",
  true, true,
  ""),


//UPDATE OBJECT TYPE on very long Object Type Name (2)
QueryAnswer(false, "UPDATE OBJECT TYPE on very long Object Type Name (2)",
  "UPDATE TYPE [otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah\n"
  "	ADD willbeadded2 : integer;\n"
  "        REMOVE willbeadded;\n"
  "]\n"
  "",
  true, true,
  ""),


//GET OBJECTS HAVING MONADS IN on very long object type name
QueryAnswer(false, "GET OBJECTS HAVING MONADS IN on very long object type name",
  "GET OBJECTS\n"
  "HAVING MONADS IN {1-100000 }\n"
  "[otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah\n"
  "GET all]\n"
  "",
  true, true,
  " // <  < [ otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah 11000002 { 1 } false (mylistofenum=\" VP NP \",mylistofid_d=\" 1 2 3 \",mylistofinteger=\" 1 2 3 \",myenum=\"NP\",mysom=\" { 1-10 } \",mystringfromset=\"BLAH\",mystring=\"Blah blah\",myascii=\"Blah\",myid_d=\"0\",myinteger=\"100\",self=\"11000002\",willbeadded2=\"0\") //  <  > \n"
  " ]\n"
  "[ otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah 11000003 { 2 } false (mylistofenum=\" NP VP \",mylistofid_d=\" 1 2 3 3000000 \",mylistofinteger=\" 1 2 3 4000000 \",myenum=\"VP\",mysom=\" { 1-12 } \",mystringfromset=\"BLAH2\",mystring=\"Blah2 blah2\",myascii=\"Blah2\",myid_d=\"0\",myinteger=\"101\",self=\"11000003\",willbeadded2=\"0\") //  <  > \n"
  " ]\n"
  "[ otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah 11000004 { 3 } false (mylistofenum=\"\",mylistofid_d=\" 1 2 3 \",mylistofinteger=\" 1 2 3 \",myenum=\"NP\",mysom=\" { 1-10 } \",mystringfromset=\"BLAH\",mystring=\"Blah blah\",myascii=\"Blah\",myid_d=\"0\",myinteger=\"100\",self=\"11000004\",willbeadded2=\"0\") //  <  > \n"
  " ]\n"
  "[ otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah 11000005 { 4 } false (mylistofenum=\" VP \",mylistofid_d=\"\",mylistofinteger=\" 1 2 3 \",myenum=\"NP\",mysom=\" { 4 } \",mystringfromset=\"BLAH\",mystring=\"Blah blah\",myascii=\"Blah\",myid_d=\"0\",myinteger=\"100\",self=\"11000005\",willbeadded2=\"0\") //  <  > \n"
  " ]\n"
  " > \n"
  " >\n"
  ""),


//SELECT ALL OBJECTS on very long object type name
QueryAnswer(false, "SELECT ALL OBJECTS on very long object type name",
  "SELECT ALL OBJECTS\n"
  "IN {1-100000 }\n"
  "WHERE\n"
  "[otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah\n"
  "GET\n"
  " myinteger,\n"
  " myid_d,\n"
  " myascii,\n"
  " mystring,\n"
  " mystringfromset,\n"
  " mysom,\n"
  " myenum,\n"
  " mylistofinteger,\n"
  " mylistofid_d,\n"
  " mylistofenum\n"
  " ]\n"
  "",
  true, true,
  " //  <  < [ otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah 11000002 { 1 } false (myinteger=\"100\",myid_d=\"0\",myascii=\"Blah\",mystring=\"Blah blah\",mystringfromset=\"BLAH\",mysom=\" { 1-10 } \",myenum=\"NP\",mylistofinteger=\" 1 2 3 \",mylistofid_d=\" 1 2 3 \",mylistofenum=\" VP NP \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah 11000003 { 2 } false (myinteger=\"101\",myid_d=\"0\",myascii=\"Blah2\",mystring=\"Blah2 blah2\",mystringfromset=\"BLAH2\",mysom=\" { 1-12 } \",myenum=\"VP\",mylistofinteger=\" 1 2 3 4000000 \",mylistofid_d=\" 1 2 3 3000000 \",mylistofenum=\" NP VP \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah 11000004 { 3 } false (myinteger=\"100\",myid_d=\"0\",myascii=\"Blah\",mystring=\"Blah blah\",mystringfromset=\"BLAH\",mysom=\" { 1-10 } \",myenum=\"NP\",mylistofinteger=\" 1 2 3 \",mylistofid_d=\" 1 2 3 \",mylistofenum=\"\") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah 11000005 { 4 } false (myinteger=\"100\",myid_d=\"0\",myascii=\"Blah\",mystring=\"Blah blah\",mystringfromset=\"BLAH\",mysom=\" { 4 } \",myenum=\"NP\",mylistofinteger=\" 1 2 3 \",mylistofid_d=\"\",mylistofenum=\" VP \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//SELECT FEATURES FROM an object type with a very long name
QueryAnswer(false, "SELECT FEATURES FROM an object type with a very long name",
  "SELECT FEATURES\n"
  "FROM OBJECT TYPE\n"
  "[otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah]\n"
  "GO\n"
  "",
  true, true,
  "+-----------------------+-----------------------+------------------+-----------------+\n"
  "| feature name : string | type : string         | default : string | computed : bool |\n"
  "+-----------------------+-----------------------+------------------+-----------------+\n"
  "| mylistofenum          | list of phrase_type_t | ''               | false           |\n"
  "| mylistofid_d          | list of id_d          | ''               | false           |\n"
  "| mylistofinteger       | list of integer       | ''               | false           |\n"
  "| myenum                | phrase_type_t         | 'NP'             | false           |\n"
  "| mysom                 | set of monads         | ' {  } '         | false           |\n"
  "| mystringfromset       | string                | ''               | false           |\n"
  "| mystring              | string                | ''               | false           |\n"
  "| myascii               | ascii                 | ''               | false           |\n"
  "| myid_d                | id_d                  | '0'              | false           |\n"
  "| myinteger             | integer               | '0'              | false           |\n"
  "| self                  | id_d                  | '0'              | true            |\n"
  "| willbeadded2          | integer               | '0'              | false           |\n"
  "+-----------------------+-----------------------+------------------+-----------------+\n"
  ""),


//DROP OBJECT TYPE on very long-named object type
QueryAnswer(false, "DROP OBJECT TYPE on very long-named object type",
  "DROP OBJECT TYPE\n"
  "[otn_3fc9d95dd5a854b05a28ab398468d0128d646062c2262cd2933c5b7fd31fe45e_c2d3738c477fc43fc372a94dad2c08d1904127383d9fcf5518ca774ad21ada2a_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_3c1ec19080a981e00f915798113abcf454fa05b0911ac5d9aa5cc7442c079f74_blah_blah]\n"
  "GO\n"
  "",
  true, true,
  ""),


//Creating DB from ASV Genesis chapter 1:1-10
QueryAnswer(true, "Creating DB from ASV Genesis chapter 1:1-10",
  "//////////////////////////////////////////////////// \n"
  "// \n"
  "// Create object types\n"
  "// \n"
  "//////////////////////////////////////////////////// \n"
  "// Enumeration divtype_e\n"
  "CREATE ENUMERATION divtype_e = {\n"
  "  book = 2,\n"
  "  DEFAULT bookGroup = 1,\n"
  "  chapter = 3\n"
  "}\n"
  "GO\n"
  "\n"
  "\n"
  "// Object type verse\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[verse\n"
  "  chapter : integer DEFAULT 0;\n"
  "  verse : integer DEFAULT 0;\n"
  "  xmlindex : integer DEFAULT 0;\n"
  "  osisid : string DEFAULT \"\";\n"
  "  osisbook : string FROM SET  DEFAULT \"\";\n"
  "]\n"
  "GO\n"
  "\n"
  "// Object type token\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE MONAD OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[token\n"
  "  xmlindex : integer DEFAULT 0;\n"
  "  surface_stripped_lowcase : string DEFAULT \"\";\n"
  "  wholesurface : string DEFAULT \"\";\n"
  "]\n"
  "GO\n"
  "\n"
  "// Object type paragraph\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "HAVING UNIQUE FIRST MONADS\n"
  "[paragraph\n"
  "  xmlindex : integer DEFAULT 0;\n"
  "]\n"
  "GO\n"
  "\n"
  "// Object type title\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "[title\n"
  "  xmlindex : integer DEFAULT 0;\n"
  "  divtype : divtype_e DEFAULT bookGroup;\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  ""),


//CREATING ASV Verse objects
QueryAnswer(false, "CREATING ASV Verse objects",
  "CREATE OBJECTS\n"
  "WITH OBJECT TYPE[verse]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 8-17 } \n"
  "WITH ID_D=23 [\n"
  "chapter:=1;\n"
  "verse:=1;\n"
  "xmlindex:=13;\n"
  "osisid:=\" Gen.1.1 \";\n"
  "osisbook:=\"Gen\";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 18-45 } \n"
  "WITH ID_D=24 [\n"
  "chapter:=1;\n"
  "verse:=2;\n"
  "xmlindex:=24;\n"
  "osisid:=\" Gen.1.2 \";\n"
  "osisbook:=\"Gen\";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 46-56 } \n"
  "WITH ID_D=25 [\n"
  "chapter:=1;\n"
  "verse:=3;\n"
  "xmlindex:=53;\n"
  "osisid:=\" Gen.1.3 \";\n"
  "osisbook:=\"Gen\";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 57-73 } \n"
  "WITH ID_D=26 [\n"
  "chapter:=1;\n"
  "verse:=4;\n"
  "xmlindex:=65;\n"
  "osisid:=\" Gen.1.4 \";\n"
  "osisbook:=\"Gen\";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 74-95 } \n"
  "WITH ID_D=27 [\n"
  "chapter:=1;\n"
  "verse:=5;\n"
  "xmlindex:=83;\n"
  "osisid:=\" Gen.1.5 \";\n"
  "osisbook:=\"Gen\";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 96-118 } \n"
  "WITH ID_D=28 [\n"
  "chapter:=1;\n"
  "verse:=6;\n"
  "xmlindex:=107;\n"
  "osisid:=\" Gen.1.6 \";\n"
  "osisbook:=\"Gen\";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 119-144 } \n"
  "WITH ID_D=29 [\n"
  "chapter:=1;\n"
  "verse:=7;\n"
  "xmlindex:=131;\n"
  "osisid:=\" Gen.1.7 \";\n"
  "osisbook:=\"Gen\";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 145-161 } \n"
  "WITH ID_D=30 [\n"
  "chapter:=1;\n"
  "verse:=8;\n"
  "xmlindex:=158;\n"
  "osisid:=\" Gen.1.8 \";\n"
  "osisbook:=\"Gen\";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 162-186 } \n"
  "WITH ID_D=31 [\n"
  "chapter:=1;\n"
  "verse:=9;\n"
  "xmlindex:=177;\n"
  "osisid:=\" Gen.1.9 \";\n"
  "osisbook:=\"Gen\";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 187-210 } \n"
  "WITH ID_D=32 [\n"
  "chapter:=1;\n"
  "verse:=10;\n"
  "xmlindex:=203;\n"
  "osisid:=\" Gen.1.10 \";\n"
  "osisbook:=\"Gen\";\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 10                     |\n"
  "+------------------------+\n"
  ""),


//CREATING ASV Token objects
QueryAnswer(false, "CREATING ASV Token objects",
  "CREATE OBJECTS\n"
  "WITH OBJECT TYPE[token]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 1 } \n"
  "WITH ID_D=103 [\n"
  "xmlindex:=3;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"The \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 2 } \n"
  "WITH ID_D=104 [\n"
  "xmlindex:=4;\n"
  "surface_stripped_lowcase:=\"first\";\n"
  "wholesurface:=\"First \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 3 } \n"
  "WITH ID_D=105 [\n"
  "xmlindex:=5;\n"
  "surface_stripped_lowcase:=\"book\";\n"
  "wholesurface:=\"Book \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 4 } \n"
  "WITH ID_D=106 [\n"
  "xmlindex:=6;\n"
  "surface_stripped_lowcase:=\"of\";\n"
  "wholesurface:=\"of \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 5 } \n"
  "WITH ID_D=107 [\n"
  "xmlindex:=7;\n"
  "surface_stripped_lowcase:=\"moses\";\n"
  "wholesurface:=\"Moses, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 6 } \n"
  "WITH ID_D=108 [\n"
  "xmlindex:=8;\n"
  "surface_stripped_lowcase:=\"called\";\n"
  "wholesurface:=\"called \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 7 } \n"
  "WITH ID_D=109 [\n"
  "xmlindex:=9;\n"
  "surface_stripped_lowcase:=\"genesis\";\n"
  "wholesurface:=\"Genesis \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 8 } \n"
  "WITH ID_D=110 [\n"
  "xmlindex:=14;\n"
  "surface_stripped_lowcase:=\"in\";\n"
  "wholesurface:=\"In \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 9 } \n"
  "WITH ID_D=111 [\n"
  "xmlindex:=15;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 10 } \n"
  "WITH ID_D=112 [\n"
  "xmlindex:=16;\n"
  "surface_stripped_lowcase:=\"beginning\";\n"
  "wholesurface:=\"beginning \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 11 } \n"
  "WITH ID_D=113 [\n"
  "xmlindex:=17;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 12 } \n"
  "WITH ID_D=114 [\n"
  "xmlindex:=18;\n"
  "surface_stripped_lowcase:=\"created\";\n"
  "wholesurface:=\"created \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 13 } \n"
  "WITH ID_D=115 [\n"
  "xmlindex:=19;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 14 } \n"
  "WITH ID_D=116 [\n"
  "xmlindex:=20;\n"
  "surface_stripped_lowcase:=\"heavens\";\n"
  "wholesurface:=\"heavens \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 15 } \n"
  "WITH ID_D=117 [\n"
  "xmlindex:=21;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 16 } \n"
  "WITH ID_D=118 [\n"
  "xmlindex:=22;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 17 } \n"
  "WITH ID_D=119 [\n"
  "xmlindex:=23;\n"
  "surface_stripped_lowcase:=\"earth\";\n"
  "wholesurface:=\"earth. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 18 } \n"
  "WITH ID_D=120 [\n"
  "xmlindex:=25;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 19 } \n"
  "WITH ID_D=121 [\n"
  "xmlindex:=26;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 20 } \n"
  "WITH ID_D=122 [\n"
  "xmlindex:=27;\n"
  "surface_stripped_lowcase:=\"earth\";\n"
  "wholesurface:=\"earth \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 21 } \n"
  "WITH ID_D=123 [\n"
  "xmlindex:=28;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 22 } \n"
  "WITH ID_D=124 [\n"
  "xmlindex:=29;\n"
  "surface_stripped_lowcase:=\"waste\";\n"
  "wholesurface:=\"waste \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 23 } \n"
  "WITH ID_D=125 [\n"
  "xmlindex:=30;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 24 } \n"
  "WITH ID_D=126 [\n"
  "xmlindex:=31;\n"
  "surface_stripped_lowcase:=\"void\";\n"
  "wholesurface:=\"void; \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 25 } \n"
  "WITH ID_D=127 [\n"
  "xmlindex:=32;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 26 } \n"
  "WITH ID_D=128 [\n"
  "xmlindex:=33;\n"
  "surface_stripped_lowcase:=\"darkness\";\n"
  "wholesurface:=\"darkness \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 27 } \n"
  "WITH ID_D=129 [\n"
  "xmlindex:=34;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 28 } \n"
  "WITH ID_D=130 [\n"
  "xmlindex:=35;\n"
  "surface_stripped_lowcase:=\"upon\";\n"
  "wholesurface:=\"upon \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 29 } \n"
  "WITH ID_D=131 [\n"
  "xmlindex:=36;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 30 } \n"
  "WITH ID_D=132 [\n"
  "xmlindex:=37;\n"
  "surface_stripped_lowcase:=\"face\";\n"
  "wholesurface:=\"face \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 31 } \n"
  "WITH ID_D=133 [\n"
  "xmlindex:=38;\n"
  "surface_stripped_lowcase:=\"of\";\n"
  "wholesurface:=\"of \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 32 } \n"
  "WITH ID_D=134 [\n"
  "xmlindex:=39;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 33 } \n"
  "WITH ID_D=135 [\n"
  "xmlindex:=40;\n"
  "surface_stripped_lowcase:=\"deep\";\n"
  "wholesurface:=\"deep: \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 34 } \n"
  "WITH ID_D=136 [\n"
  "xmlindex:=41;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 35 } \n"
  "WITH ID_D=137 [\n"
  "xmlindex:=42;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 36 } \n"
  "WITH ID_D=138 [\n"
  "xmlindex:=43;\n"
  "surface_stripped_lowcase:=\"spirit\";\n"
  "wholesurface:=\"Spirit \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 37 } \n"
  "WITH ID_D=139 [\n"
  "xmlindex:=44;\n"
  "surface_stripped_lowcase:=\"of\";\n"
  "wholesurface:=\"of \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 38 } \n"
  "WITH ID_D=140 [\n"
  "xmlindex:=45;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 39 } \n"
  "WITH ID_D=141 [\n"
  "xmlindex:=46;\n"
  "surface_stripped_lowcase:=\"moved\";\n"
  "wholesurface:=\"moved \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 40 } \n"
  "WITH ID_D=142 [\n"
  "xmlindex:=47;\n"
  "surface_stripped_lowcase:=\"upon\";\n"
  "wholesurface:=\"upon \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 41 } \n"
  "WITH ID_D=143 [\n"
  "xmlindex:=48;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 42 } \n"
  "WITH ID_D=144 [\n"
  "xmlindex:=49;\n"
  "surface_stripped_lowcase:=\"face\";\n"
  "wholesurface:=\"face \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 43 } \n"
  "WITH ID_D=145 [\n"
  "xmlindex:=50;\n"
  "surface_stripped_lowcase:=\"of\";\n"
  "wholesurface:=\"of \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 44 } \n"
  "WITH ID_D=146 [\n"
  "xmlindex:=51;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 45 } \n"
  "WITH ID_D=147 [\n"
  "xmlindex:=52;\n"
  "surface_stripped_lowcase:=\"waters\";\n"
  "wholesurface:=\"waters. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 46 } \n"
  "WITH ID_D=148 [\n"
  "xmlindex:=54;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 47 } \n"
  "WITH ID_D=149 [\n"
  "xmlindex:=55;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 48 } \n"
  "WITH ID_D=150 [\n"
  "xmlindex:=56;\n"
  "surface_stripped_lowcase:=\"said\";\n"
  "wholesurface:=\"said, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 49 } \n"
  "WITH ID_D=151 [\n"
  "xmlindex:=57;\n"
  "surface_stripped_lowcase:=\"let\";\n"
  "wholesurface:=\"Let \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 50 } \n"
  "WITH ID_D=152 [\n"
  "xmlindex:=58;\n"
  "surface_stripped_lowcase:=\"there\";\n"
  "wholesurface:=\"there \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 51 } \n"
  "WITH ID_D=153 [\n"
  "xmlindex:=59;\n"
  "surface_stripped_lowcase:=\"be\";\n"
  "wholesurface:=\"be \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 52 } \n"
  "WITH ID_D=154 [\n"
  "xmlindex:=60;\n"
  "surface_stripped_lowcase:=\"light\";\n"
  "wholesurface:=\"light: \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 53 } \n"
  "WITH ID_D=155 [\n"
  "xmlindex:=61;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 54 } \n"
  "WITH ID_D=156 [\n"
  "xmlindex:=62;\n"
  "surface_stripped_lowcase:=\"there\";\n"
  "wholesurface:=\"there \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 55 } \n"
  "WITH ID_D=157 [\n"
  "xmlindex:=63;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 56 } \n"
  "WITH ID_D=158 [\n"
  "xmlindex:=64;\n"
  "surface_stripped_lowcase:=\"light\";\n"
  "wholesurface:=\"light. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 57 } \n"
  "WITH ID_D=159 [\n"
  "xmlindex:=66;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 58 } \n"
  "WITH ID_D=160 [\n"
  "xmlindex:=67;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 59 } \n"
  "WITH ID_D=161 [\n"
  "xmlindex:=68;\n"
  "surface_stripped_lowcase:=\"saw\";\n"
  "wholesurface:=\"saw \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 60 } \n"
  "WITH ID_D=162 [\n"
  "xmlindex:=69;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 61 } \n"
  "WITH ID_D=163 [\n"
  "xmlindex:=70;\n"
  "surface_stripped_lowcase:=\"light\";\n"
  "wholesurface:=\"light, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 62 } \n"
  "WITH ID_D=164 [\n"
  "xmlindex:=71;\n"
  "surface_stripped_lowcase:=\"that\";\n"
  "wholesurface:=\"that \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 63 } \n"
  "WITH ID_D=165 [\n"
  "xmlindex:=72;\n"
  "surface_stripped_lowcase:=\"it\";\n"
  "wholesurface:=\"it \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 64 } \n"
  "WITH ID_D=166 [\n"
  "xmlindex:=73;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 65 } \n"
  "WITH ID_D=167 [\n"
  "xmlindex:=74;\n"
  "surface_stripped_lowcase:=\"good\";\n"
  "wholesurface:=\"good: \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 66 } \n"
  "WITH ID_D=168 [\n"
  "xmlindex:=75;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 67 } \n"
  "WITH ID_D=169 [\n"
  "xmlindex:=76;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 68 } \n"
  "WITH ID_D=170 [\n"
  "xmlindex:=77;\n"
  "surface_stripped_lowcase:=\"divided\";\n"
  "wholesurface:=\"divided \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 69 } \n"
  "WITH ID_D=171 [\n"
  "xmlindex:=78;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 70 } \n"
  "WITH ID_D=172 [\n"
  "xmlindex:=79;\n"
  "surface_stripped_lowcase:=\"light\";\n"
  "wholesurface:=\"light \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 71 } \n"
  "WITH ID_D=173 [\n"
  "xmlindex:=80;\n"
  "surface_stripped_lowcase:=\"from\";\n"
  "wholesurface:=\"from \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 72 } \n"
  "WITH ID_D=174 [\n"
  "xmlindex:=81;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 73 } \n"
  "WITH ID_D=175 [\n"
  "xmlindex:=82;\n"
  "surface_stripped_lowcase:=\"darkness\";\n"
  "wholesurface:=\"darkness. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 74 } \n"
  "WITH ID_D=176 [\n"
  "xmlindex:=84;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 75 } \n"
  "WITH ID_D=177 [\n"
  "xmlindex:=85;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 76 } \n"
  "WITH ID_D=178 [\n"
  "xmlindex:=86;\n"
  "surface_stripped_lowcase:=\"called\";\n"
  "wholesurface:=\"called \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 77 } \n"
  "WITH ID_D=179 [\n"
  "xmlindex:=87;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 78 } \n"
  "WITH ID_D=180 [\n"
  "xmlindex:=88;\n"
  "surface_stripped_lowcase:=\"light\";\n"
  "wholesurface:=\"light \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 79 } \n"
  "WITH ID_D=181 [\n"
  "xmlindex:=89;\n"
  "surface_stripped_lowcase:=\"day\";\n"
  "wholesurface:=\"Day, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 80 } \n"
  "WITH ID_D=182 [\n"
  "xmlindex:=90;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 81 } \n"
  "WITH ID_D=183 [\n"
  "xmlindex:=91;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 82 } \n"
  "WITH ID_D=184 [\n"
  "xmlindex:=92;\n"
  "surface_stripped_lowcase:=\"darkness\";\n"
  "wholesurface:=\"darkness \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 83 } \n"
  "WITH ID_D=185 [\n"
  "xmlindex:=93;\n"
  "surface_stripped_lowcase:=\"he\";\n"
  "wholesurface:=\"he \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 84 } \n"
  "WITH ID_D=186 [\n"
  "xmlindex:=94;\n"
  "surface_stripped_lowcase:=\"called\";\n"
  "wholesurface:=\"called \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 85 } \n"
  "WITH ID_D=187 [\n"
  "xmlindex:=95;\n"
  "surface_stripped_lowcase:=\"night\";\n"
  "wholesurface:=\"Night. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 86 } \n"
  "WITH ID_D=188 [\n"
  "xmlindex:=96;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 87 } \n"
  "WITH ID_D=189 [\n"
  "xmlindex:=97;\n"
  "surface_stripped_lowcase:=\"there\";\n"
  "wholesurface:=\"there \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 88 } \n"
  "WITH ID_D=190 [\n"
  "xmlindex:=98;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 89 } \n"
  "WITH ID_D=191 [\n"
  "xmlindex:=99;\n"
  "surface_stripped_lowcase:=\"evening\";\n"
  "wholesurface:=\"evening \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 90 } \n"
  "WITH ID_D=192 [\n"
  "xmlindex:=100;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 91 } \n"
  "WITH ID_D=193 [\n"
  "xmlindex:=101;\n"
  "surface_stripped_lowcase:=\"there\";\n"
  "wholesurface:=\"there \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 92 } \n"
  "WITH ID_D=194 [\n"
  "xmlindex:=102;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 93 } \n"
  "WITH ID_D=195 [\n"
  "xmlindex:=103;\n"
  "surface_stripped_lowcase:=\"morning\";\n"
  "wholesurface:=\"morning, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 94 } \n"
  "WITH ID_D=196 [\n"
  "xmlindex:=104;\n"
  "surface_stripped_lowcase:=\"one\";\n"
  "wholesurface:=\"one \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 95 } \n"
  "WITH ID_D=197 [\n"
  "xmlindex:=105;\n"
  "surface_stripped_lowcase:=\"day\";\n"
  "wholesurface:=\"day. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 96 } \n"
  "WITH ID_D=198 [\n"
  "xmlindex:=108;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 97 } \n"
  "WITH ID_D=199 [\n"
  "xmlindex:=109;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 98 } \n"
  "WITH ID_D=200 [\n"
  "xmlindex:=110;\n"
  "surface_stripped_lowcase:=\"said\";\n"
  "wholesurface:=\"said, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 99 } \n"
  "WITH ID_D=201 [\n"
  "xmlindex:=111;\n"
  "surface_stripped_lowcase:=\"let\";\n"
  "wholesurface:=\"Let \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 100 } \n"
  "WITH ID_D=202 [\n"
  "xmlindex:=112;\n"
  "surface_stripped_lowcase:=\"there\";\n"
  "wholesurface:=\"there \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 101 } \n"
  "WITH ID_D=203 [\n"
  "xmlindex:=113;\n"
  "surface_stripped_lowcase:=\"be\";\n"
  "wholesurface:=\"be \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 102 } \n"
  "WITH ID_D=204 [\n"
  "xmlindex:=114;\n"
  "surface_stripped_lowcase:=\"a\";\n"
  "wholesurface:=\"a \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 103 } \n"
  "WITH ID_D=205 [\n"
  "xmlindex:=115;\n"
  "surface_stripped_lowcase:=\"firmament\";\n"
  "wholesurface:=\"firmament \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 104 } \n"
  "WITH ID_D=206 [\n"
  "xmlindex:=116;\n"
  "surface_stripped_lowcase:=\"in\";\n"
  "wholesurface:=\"in \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 105 } \n"
  "WITH ID_D=207 [\n"
  "xmlindex:=117;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 106 } \n"
  "WITH ID_D=208 [\n"
  "xmlindex:=118;\n"
  "surface_stripped_lowcase:=\"midst\";\n"
  "wholesurface:=\"midst \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 107 } \n"
  "WITH ID_D=209 [\n"
  "xmlindex:=119;\n"
  "surface_stripped_lowcase:=\"of\";\n"
  "wholesurface:=\"of \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 108 } \n"
  "WITH ID_D=210 [\n"
  "xmlindex:=120;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 109 } \n"
  "WITH ID_D=211 [\n"
  "xmlindex:=121;\n"
  "surface_stripped_lowcase:=\"waters\";\n"
  "wholesurface:=\"waters, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 110 } \n"
  "WITH ID_D=212 [\n"
  "xmlindex:=122;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 111 } \n"
  "WITH ID_D=213 [\n"
  "xmlindex:=123;\n"
  "surface_stripped_lowcase:=\"let\";\n"
  "wholesurface:=\"let \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 112 } \n"
  "WITH ID_D=214 [\n"
  "xmlindex:=124;\n"
  "surface_stripped_lowcase:=\"it\";\n"
  "wholesurface:=\"it \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 113 } \n"
  "WITH ID_D=215 [\n"
  "xmlindex:=125;\n"
  "surface_stripped_lowcase:=\"divide\";\n"
  "wholesurface:=\"divide \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 114 } \n"
  "WITH ID_D=216 [\n"
  "xmlindex:=126;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 115 } \n"
  "WITH ID_D=217 [\n"
  "xmlindex:=127;\n"
  "surface_stripped_lowcase:=\"waters\";\n"
  "wholesurface:=\"waters \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 116 } \n"
  "WITH ID_D=218 [\n"
  "xmlindex:=128;\n"
  "surface_stripped_lowcase:=\"from\";\n"
  "wholesurface:=\"from \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 117 } \n"
  "WITH ID_D=219 [\n"
  "xmlindex:=129;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 118 } \n"
  "WITH ID_D=220 [\n"
  "xmlindex:=130;\n"
  "surface_stripped_lowcase:=\"waters\";\n"
  "wholesurface:=\"waters. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 119 } \n"
  "WITH ID_D=221 [\n"
  "xmlindex:=132;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 120 } \n"
  "WITH ID_D=222 [\n"
  "xmlindex:=133;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 121 } \n"
  "WITH ID_D=223 [\n"
  "xmlindex:=134;\n"
  "surface_stripped_lowcase:=\"made\";\n"
  "wholesurface:=\"made \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 122 } \n"
  "WITH ID_D=224 [\n"
  "xmlindex:=135;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 123 } \n"
  "WITH ID_D=225 [\n"
  "xmlindex:=136;\n"
  "surface_stripped_lowcase:=\"firmament\";\n"
  "wholesurface:=\"firmament, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 124 } \n"
  "WITH ID_D=226 [\n"
  "xmlindex:=137;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 125 } \n"
  "WITH ID_D=227 [\n"
  "xmlindex:=138;\n"
  "surface_stripped_lowcase:=\"divided\";\n"
  "wholesurface:=\"divided \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 126 } \n"
  "WITH ID_D=228 [\n"
  "xmlindex:=139;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 127 } \n"
  "WITH ID_D=229 [\n"
  "xmlindex:=140;\n"
  "surface_stripped_lowcase:=\"waters\";\n"
  "wholesurface:=\"waters \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 128 } \n"
  "WITH ID_D=230 [\n"
  "xmlindex:=141;\n"
  "surface_stripped_lowcase:=\"which\";\n"
  "wholesurface:=\"which \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 129 } \n"
  "WITH ID_D=231 [\n"
  "xmlindex:=142;\n"
  "surface_stripped_lowcase:=\"were\";\n"
  "wholesurface:=\"were \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 130 } \n"
  "WITH ID_D=232 [\n"
  "xmlindex:=143;\n"
  "surface_stripped_lowcase:=\"under\";\n"
  "wholesurface:=\"under \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 131 } \n"
  "WITH ID_D=233 [\n"
  "xmlindex:=144;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 132 } \n"
  "WITH ID_D=234 [\n"
  "xmlindex:=145;\n"
  "surface_stripped_lowcase:=\"firmament\";\n"
  "wholesurface:=\"firmament \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 133 } \n"
  "WITH ID_D=235 [\n"
  "xmlindex:=146;\n"
  "surface_stripped_lowcase:=\"from\";\n"
  "wholesurface:=\"from \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 134 } \n"
  "WITH ID_D=236 [\n"
  "xmlindex:=147;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 135 } \n"
  "WITH ID_D=237 [\n"
  "xmlindex:=148;\n"
  "surface_stripped_lowcase:=\"waters\";\n"
  "wholesurface:=\"waters \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 136 } \n"
  "WITH ID_D=238 [\n"
  "xmlindex:=149;\n"
  "surface_stripped_lowcase:=\"which\";\n"
  "wholesurface:=\"which \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 137 } \n"
  "WITH ID_D=239 [\n"
  "xmlindex:=150;\n"
  "surface_stripped_lowcase:=\"were\";\n"
  "wholesurface:=\"were \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 138 } \n"
  "WITH ID_D=240 [\n"
  "xmlindex:=151;\n"
  "surface_stripped_lowcase:=\"above\";\n"
  "wholesurface:=\"above \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 139 } \n"
  "WITH ID_D=241 [\n"
  "xmlindex:=152;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 140 } \n"
  "WITH ID_D=242 [\n"
  "xmlindex:=153;\n"
  "surface_stripped_lowcase:=\"firmament\";\n"
  "wholesurface:=\"firmament: \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 141 } \n"
  "WITH ID_D=243 [\n"
  "xmlindex:=154;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 142 } \n"
  "WITH ID_D=244 [\n"
  "xmlindex:=155;\n"
  "surface_stripped_lowcase:=\"it\";\n"
  "wholesurface:=\"it \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 143 } \n"
  "WITH ID_D=245 [\n"
  "xmlindex:=156;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 144 } \n"
  "WITH ID_D=246 [\n"
  "xmlindex:=157;\n"
  "surface_stripped_lowcase:=\"so\";\n"
  "wholesurface:=\"so. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 145 } \n"
  "WITH ID_D=247 [\n"
  "xmlindex:=159;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 146 } \n"
  "WITH ID_D=248 [\n"
  "xmlindex:=160;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 147 } \n"
  "WITH ID_D=249 [\n"
  "xmlindex:=161;\n"
  "surface_stripped_lowcase:=\"called\";\n"
  "wholesurface:=\"called \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 148 } \n"
  "WITH ID_D=250 [\n"
  "xmlindex:=162;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 149 } \n"
  "WITH ID_D=251 [\n"
  "xmlindex:=163;\n"
  "surface_stripped_lowcase:=\"firmament\";\n"
  "wholesurface:=\"firmament \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 150 } \n"
  "WITH ID_D=252 [\n"
  "xmlindex:=164;\n"
  "surface_stripped_lowcase:=\"heaven\";\n"
  "wholesurface:=\"Heaven. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 151 } \n"
  "WITH ID_D=253 [\n"
  "xmlindex:=165;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 152 } \n"
  "WITH ID_D=254 [\n"
  "xmlindex:=166;\n"
  "surface_stripped_lowcase:=\"there\";\n"
  "wholesurface:=\"there \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 153 } \n"
  "WITH ID_D=255 [\n"
  "xmlindex:=167;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 154 } \n"
  "WITH ID_D=256 [\n"
  "xmlindex:=168;\n"
  "surface_stripped_lowcase:=\"evening\";\n"
  "wholesurface:=\"evening \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 155 } \n"
  "WITH ID_D=257 [\n"
  "xmlindex:=169;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 156 } \n"
  "WITH ID_D=258 [\n"
  "xmlindex:=170;\n"
  "surface_stripped_lowcase:=\"there\";\n"
  "wholesurface:=\"there \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 157 } \n"
  "WITH ID_D=259 [\n"
  "xmlindex:=171;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 158 } \n"
  "WITH ID_D=260 [\n"
  "xmlindex:=172;\n"
  "surface_stripped_lowcase:=\"morning\";\n"
  "wholesurface:=\"morning, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 159 } \n"
  "WITH ID_D=261 [\n"
  "xmlindex:=173;\n"
  "surface_stripped_lowcase:=\"a\";\n"
  "wholesurface:=\"a \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 160 } \n"
  "WITH ID_D=262 [\n"
  "xmlindex:=174;\n"
  "surface_stripped_lowcase:=\"second\";\n"
  "wholesurface:=\"second \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 161 } \n"
  "WITH ID_D=263 [\n"
  "xmlindex:=175;\n"
  "surface_stripped_lowcase:=\"day\";\n"
  "wholesurface:=\"day. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 162 } \n"
  "WITH ID_D=264 [\n"
  "xmlindex:=178;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 163 } \n"
  "WITH ID_D=265 [\n"
  "xmlindex:=179;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 164 } \n"
  "WITH ID_D=266 [\n"
  "xmlindex:=180;\n"
  "surface_stripped_lowcase:=\"said\";\n"
  "wholesurface:=\"said, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 165 } \n"
  "WITH ID_D=267 [\n"
  "xmlindex:=181;\n"
  "surface_stripped_lowcase:=\"let\";\n"
  "wholesurface:=\"Let \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 166 } \n"
  "WITH ID_D=268 [\n"
  "xmlindex:=182;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 167 } \n"
  "WITH ID_D=269 [\n"
  "xmlindex:=183;\n"
  "surface_stripped_lowcase:=\"waters\";\n"
  "wholesurface:=\"waters \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 168 } \n"
  "WITH ID_D=270 [\n"
  "xmlindex:=184;\n"
  "surface_stripped_lowcase:=\"under\";\n"
  "wholesurface:=\"under \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 169 } \n"
  "WITH ID_D=271 [\n"
  "xmlindex:=185;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 170 } \n"
  "WITH ID_D=272 [\n"
  "xmlindex:=186;\n"
  "surface_stripped_lowcase:=\"heavens\";\n"
  "wholesurface:=\"heavens \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 171 } \n"
  "WITH ID_D=273 [\n"
  "xmlindex:=187;\n"
  "surface_stripped_lowcase:=\"be\";\n"
  "wholesurface:=\"be \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 172 } \n"
  "WITH ID_D=274 [\n"
  "xmlindex:=188;\n"
  "surface_stripped_lowcase:=\"gathered\";\n"
  "wholesurface:=\"gathered \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 173 } \n"
  "WITH ID_D=275 [\n"
  "xmlindex:=189;\n"
  "surface_stripped_lowcase:=\"together\";\n"
  "wholesurface:=\"together \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 174 } \n"
  "WITH ID_D=276 [\n"
  "xmlindex:=190;\n"
  "surface_stripped_lowcase:=\"unto\";\n"
  "wholesurface:=\"unto \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 175 } \n"
  "WITH ID_D=277 [\n"
  "xmlindex:=191;\n"
  "surface_stripped_lowcase:=\"one\";\n"
  "wholesurface:=\"one \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 176 } \n"
  "WITH ID_D=278 [\n"
  "xmlindex:=192;\n"
  "surface_stripped_lowcase:=\"place\";\n"
  "wholesurface:=\"place, \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 177 } \n"
  "WITH ID_D=279 [\n"
  "xmlindex:=193;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 178 } \n"
  "WITH ID_D=280 [\n"
  "xmlindex:=194;\n"
  "surface_stripped_lowcase:=\"let\";\n"
  "wholesurface:=\"let \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 179 } \n"
  "WITH ID_D=281 [\n"
  "xmlindex:=195;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 180 } \n"
  "WITH ID_D=282 [\n"
  "xmlindex:=196;\n"
  "surface_stripped_lowcase:=\"dry\";\n"
  "wholesurface:=\"dry \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 181 } \n"
  "WITH ID_D=283 [\n"
  "xmlindex:=197;\n"
  "surface_stripped_lowcase:=\"land\";\n"
  "wholesurface:=\"land \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 182 } \n"
  "WITH ID_D=284 [\n"
  "xmlindex:=198;\n"
  "surface_stripped_lowcase:=\"appear\";\n"
  "wholesurface:=\"appear: \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 183 } \n"
  "WITH ID_D=285 [\n"
  "xmlindex:=199;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 184 } \n"
  "WITH ID_D=286 [\n"
  "xmlindex:=200;\n"
  "surface_stripped_lowcase:=\"it\";\n"
  "wholesurface:=\"it \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 185 } \n"
  "WITH ID_D=287 [\n"
  "xmlindex:=201;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 186 } \n"
  "WITH ID_D=288 [\n"
  "xmlindex:=202;\n"
  "surface_stripped_lowcase:=\"so\";\n"
  "wholesurface:=\"so. \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 187 } \n"
  "WITH ID_D=289 [\n"
  "xmlindex:=204;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"And \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 188 } \n"
  "WITH ID_D=290 [\n"
  "xmlindex:=205;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 189 } \n"
  "WITH ID_D=291 [\n"
  "xmlindex:=206;\n"
  "surface_stripped_lowcase:=\"called\";\n"
  "wholesurface:=\"called \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 190 } \n"
  "WITH ID_D=292 [\n"
  "xmlindex:=207;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 191 } \n"
  "WITH ID_D=293 [\n"
  "xmlindex:=208;\n"
  "surface_stripped_lowcase:=\"dry\";\n"
  "wholesurface:=\"dry \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 192 } \n"
  "WITH ID_D=294 [\n"
  "xmlindex:=209;\n"
  "surface_stripped_lowcase:=\"land\";\n"
  "wholesurface:=\"land \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 193 } \n"
  "WITH ID_D=295 [\n"
  "xmlindex:=210;\n"
  "surface_stripped_lowcase:=\"earth\";\n"
  "wholesurface:=\"Earth; \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 194 } \n"
  "WITH ID_D=296 [\n"
  "xmlindex:=211;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 195 } \n"
  "WITH ID_D=297 [\n"
  "xmlindex:=212;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 196 } \n"
  "WITH ID_D=298 [\n"
  "xmlindex:=213;\n"
  "surface_stripped_lowcase:=\"gathering\";\n"
  "wholesurface:=\"gathering \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 197 } \n"
  "WITH ID_D=299 [\n"
  "xmlindex:=214;\n"
  "surface_stripped_lowcase:=\"together\";\n"
  "wholesurface:=\"together \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 198 } \n"
  "WITH ID_D=300 [\n"
  "xmlindex:=215;\n"
  "surface_stripped_lowcase:=\"of\";\n"
  "wholesurface:=\"of \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 199 } \n"
  "WITH ID_D=301 [\n"
  "xmlindex:=216;\n"
  "surface_stripped_lowcase:=\"the\";\n"
  "wholesurface:=\"the \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 200 } \n"
  "WITH ID_D=302 [\n"
  "xmlindex:=217;\n"
  "surface_stripped_lowcase:=\"waters\";\n"
  "wholesurface:=\"waters \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 201 } \n"
  "WITH ID_D=303 [\n"
  "xmlindex:=218;\n"
  "surface_stripped_lowcase:=\"called\";\n"
  "wholesurface:=\"called \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 202 } \n"
  "WITH ID_D=304 [\n"
  "xmlindex:=219;\n"
  "surface_stripped_lowcase:=\"he\";\n"
  "wholesurface:=\"he \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 203 } \n"
  "WITH ID_D=305 [\n"
  "xmlindex:=220;\n"
  "surface_stripped_lowcase:=\"seas\";\n"
  "wholesurface:=\"Seas: \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 204 } \n"
  "WITH ID_D=306 [\n"
  "xmlindex:=221;\n"
  "surface_stripped_lowcase:=\"and\";\n"
  "wholesurface:=\"and \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 205 } \n"
  "WITH ID_D=307 [\n"
  "xmlindex:=222;\n"
  "surface_stripped_lowcase:=\"god\";\n"
  "wholesurface:=\"God \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 206 } \n"
  "WITH ID_D=308 [\n"
  "xmlindex:=223;\n"
  "surface_stripped_lowcase:=\"saw\";\n"
  "wholesurface:=\"saw \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 207 } \n"
  "WITH ID_D=309 [\n"
  "xmlindex:=224;\n"
  "surface_stripped_lowcase:=\"that\";\n"
  "wholesurface:=\"that \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 208 } \n"
  "WITH ID_D=310 [\n"
  "xmlindex:=225;\n"
  "surface_stripped_lowcase:=\"it\";\n"
  "wholesurface:=\"it \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 209 } \n"
  "WITH ID_D=311 [\n"
  "xmlindex:=226;\n"
  "surface_stripped_lowcase:=\"was\";\n"
  "wholesurface:=\"was \";\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 210 } \n"
  "WITH ID_D=312 [\n"
  "xmlindex:=227;\n"
  "surface_stripped_lowcase:=\"good\";\n"
  "wholesurface:=\"good. \";\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 210                    |\n"
  "+------------------------+\n"
  ""),


//CREATING ASV Paragraph objects
QueryAnswer(false, "CREATING ASV Paragraph objects",
  "CREATE OBJECTS\n"
  "WITH OBJECT TYPE[paragraph]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 8-95 } \n"
  "WITH ID_D=7 [\n"
  "xmlindex:=12;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 96-161 } \n"
  "WITH ID_D=8 [\n"
  "xmlindex:=106;\n"
  "]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 162-283 } \n"
  "WITH ID_D=9 [\n"
  "xmlindex:=176;\n"
  "]\n"
  "GO\n"
  "\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 3                      |\n"
  "+------------------------+\n"
  ""),


//CREATING ASV Title objects
QueryAnswer(false, "CREATING ASV Title objects",
  "CREATE OBJECTS\n"
  "WITH OBJECT TYPE[title]\n"
  "CREATE OBJECT\n"
  "FROM MONADS= { 1-7 } \n"
  "WITH ID_D=19 [\n"
  "xmlindex:=2;\n"
  "divtype:=book;\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  "+------------------------+\n"
  "| object_count : integer |\n"
  "+------------------------+\n"
  "| 1                      |\n"
  "+------------------------+\n"
  ""),


//GET AGGREGATE FEATURES (1)
QueryAnswer(false, "GET AGGREGATE FEATURES (1)",
  "GET AGGREGATE FEATURES\n"
  "COUNT(*)\n"
  "FROM OBJECTS\n"
  "[Token]\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+\n"
  "| Column1 : integer |\n"
  "+-------------------+\n"
  "| 210               |\n"
  "+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (2)
QueryAnswer(false, "GET AGGREGATE FEATURES (2)",
  "GET AGGREGATE FEATURES\n"
  "COUNT(*), COUNT(surface_stripped_lowcase=\"god\")\n"
  "FROM OBJECTS\n"
  "[Token]\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+-------------------+\n"
  "| Column1 : integer | Column2 : integer |\n"
  "+-------------------+-------------------+\n"
  "| 210               | 12                |\n"
  "+-------------------+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (3)
QueryAnswer(false, "GET AGGREGATE FEATURES (3)",
  "GET AGGREGATE FEATURES\n"
  "COUNT(*)\n"
  "FROM OBJECTS\n"
  "[Token xmlindex<=105]\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+\n"
  "| Column1 : integer |\n"
  "+-------------------+\n"
  "| 95                |\n"
  "+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (4)
QueryAnswer(false, "GET AGGREGATE FEATURES (4)",
  "GET AGGREGATE FEATURES\n"
  "COUNT(*), COUNT(surface_stripped_lowcase='the')\n"
  "FROM OBJECTS\n"
  "[Token xmlindex<=105 AND xmlindex >= 100]\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+-------------------+\n"
  "| Column1 : integer | Column2 : integer |\n"
  "+-------------------+-------------------+\n"
  "| 6                 | 0                 |\n"
  "+-------------------+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (5)
QueryAnswer(false, "GET AGGREGATE FEATURES (5)",
  "GET AGGREGATE FEATURES\n"
  "COUNT(*), COUNT(surface_stripped_lowcase='the')\n"
  "FROM OBJECTS\n"
  "[Token xmlindex<=105 AND xmlindex >= 100]\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+-------------------+\n"
  "| Column1 : integer | Column2 : integer |\n"
  "+-------------------+-------------------+\n"
  "| 6                 | 0                 |\n"
  "+-------------------+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (6)
QueryAnswer(false, "GET AGGREGATE FEATURES (6)",
  "GET AGGREGATE FEATURES\n"
  "COUNT(surface_stripped_lowcase='one'), SUM(xmlindex), MIN(xmlindex), MAX(xmlindex)\n"
  "FROM OBJECTS\n"
  "[Token xmlindex<=105 AND xmlindex >= 100]\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+-------------------+-------------------+-------------------+\n"
  "| Column1 : integer | Column2 : integer | Column3 : integer | Column4 : integer |\n"
  "+-------------------+-------------------+-------------------+-------------------+\n"
  "| 1                 | 615               | 100               | 105               |\n"
  "+-------------------+-------------------+-------------------+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (7)
QueryAnswer(false, "GET AGGREGATE FEATURES (7)",
  "GET AGGREGATE FEATURES\n"
  "COUNT(surface_stripped_lowcase='day'), SUM(xmlindex), MIN(xmlindex), MAX(xmlindex)\n"
  "FROM OBJECTS\n"
  "[Token xmlindex<=105 OR surface_stripped_lowcase='god']\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+-------------------+-------------------+-------------------+\n"
  "| Column1 : integer | Column2 : integer | Column3 : integer | Column4 : integer |\n"
  "+-------------------+-------------------+-------------------+-------------------+\n"
  "| 2                 | 6299              | 3                 | 222               |\n"
  "+-------------------+-------------------+-------------------+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (8)
QueryAnswer(false, "GET AGGREGATE FEATURES (8)",
  "GET AGGREGATE FEATURES\n"
  "COUNT(surface_stripped_lowcase='god'), SUM(xmlindex), MIN(xmlindex), MAX(xmlindex), COUNT(surface_stripped_lowcase<>'god')\n"
  "FROM OBJECTS\n"
  "[Token surface_stripped_lowcase='god']\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+-------------------+-------------------+-------------------+-------------------+\n"
  "| Column1 : integer | Column2 : integer | Column3 : integer | Column4 : integer | Column5 : integer |\n"
  "+-------------------+-------------------+-------------------+-------------------+-------------------+\n"
  "| 12                | 1353              | 17                | 222               | 0                 |\n"
  "+-------------------+-------------------+-------------------+-------------------+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (9)
QueryAnswer(false, "GET AGGREGATE FEATURES (9)",
  "GET AGGREGATE FEATURES\n"
  "COUNT(*)\n"
  "FROM OBJECTS\n"
  "[Verse]\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+\n"
  "| Column1 : integer |\n"
  "+-------------------+\n"
  "| 10                |\n"
  "+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (10)
QueryAnswer(false, "GET AGGREGATE FEATURES (10)",
  "GET AGGREGATE FEATURES\n"
  "SUM(verse), COUNT(chapter=1)\n"
  "FROM OBJECTS\n"
  "[Verse]\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+-------------------+\n"
  "| Column1 : integer | Column2 : integer |\n"
  "+-------------------+-------------------+\n"
  "| 55                | 10                |\n"
  "+-------------------+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (11)
QueryAnswer(false, "GET AGGREGATE FEATURES (11)",
  "GET AGGREGATE FEATURES\n"
  "SUM(verse), COUNT(chapter=1)\n"
  "FROM OBJECTS\n"
  "[Verse chapter >= 1]\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+-------------------+\n"
  "| Column1 : integer | Column2 : integer |\n"
  "+-------------------+-------------------+\n"
  "| 55                | 10                |\n"
  "+-------------------+-------------------+\n"
  ""),


//GET AGGREGATE FEATURES (12)
QueryAnswer(false, "GET AGGREGATE FEATURES (12)",
  "GET AGGREGATE FEATURES\n"
  "SUM(verse), COUNT(chapter=1), COUNT(verse<=5), MAX(verse), MIN(verse), MIN(chapter), MAX(chapter)\n"
  "FROM OBJECTS\n"
  "[Verse verse >= 2 AND verse<=8]\n"
  "GO\n"
  "",
  true, true,
  "+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+\n"
  "| Column1 : integer | Column2 : integer | Column3 : integer | Column4 : integer | Column5 : integer | Column6 : integer | Column7 : integer |\n"
  "+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+\n"
  "| 35                | 7                 | 4                 | 8                 | 2                 | 1                 | 1                 |\n"
  "+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+\n"
  ""),


//illegal UnorderedGroup (1)
QueryAnswer(false, "illegal UnorderedGroup (1)",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Verse AS v1\n"
  "    [UnorderedGroup\n"
  "         [Token AS r1]\n"
  "         [Token wholesurface=r1.wholesurface]\n"
  "    ]\n"
  "]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'r1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//illegal UnorderedGroup (2)
QueryAnswer(false, "illegal UnorderedGroup (2)",
  "SELECT ALL OBJECTS\n"
  "WHERE\n"
  "[Verse AS v1\n"
  "    [UnorderedGroup\n"
  "         [Token AS r1]\n"
  "    ]\n"
  "]\n"
  "[Token wholesurface=r1.wholesurface AND self > v1.self]\n"
  "",
  true, false,
  "Symbolchecking failed with compiler error\n"
  "You have used the object reference 'r1' in such as way as to cross\n"
  "an \"export barrier\". An export barrier is either an OR construct,\n"
  "a STAR construct, or an UnorderedGroup construct.\n"
  ""),


//UNORDERED GROUP (1)
QueryAnswer(false, "UNORDERED GROUP (1)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[Verse\n"
  "	[UnorderedGroup\n"
  "		[Token surface_stripped_lowcase='god' GET wholesurface]\n"
  "		[Token surface_stripped_lowcase='good' GET wholesurface]\n"
  "	]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ verse 26 { 57-73 } false  //  <  < [ token 160 { 58 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 169 { 67 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 167 { 65 } false (wholesurface=\"good: \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 32 { 187-210 } false  //  <  < [ token 290 { 188 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 307 { 205 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 312 { 210 } false (wholesurface=\"good. \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//UNORDERED GROUP (2)
QueryAnswer(false, "UNORDERED GROUP (2)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[Verse AS v1\n"
  "	[UnorderedGroup\n"
  "		[Token surface_stripped_lowcase='god' AND self > v1.self GET wholesurface]\n"
  "		[Token surface_stripped_lowcase='good' AND self > v1.self GET wholesurface]\n"
  "	]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ verse 26 { 57-73 } false  //  <  < [ token 160 { 58 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 169 { 67 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 167 { 65 } false (wholesurface=\"good: \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 32 { 187-210 } false  //  <  < [ token 290 { 188 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 307 { 205 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 312 { 210 } false (wholesurface=\"good. \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//UNORDERED GROUP (3)
QueryAnswer(false, "UNORDERED GROUP (3)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[Verse AS v1\n"
  "	[UnorderedGroup\n"
  "		[Token surface_stripped_lowcase='god' AND self > v1.self GET wholesurface]\n"
  "		[Token surface_stripped_lowcase='made' GET wholesurface]\n"
  "	]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ verse 29 { 119-144 } false  //  <  < [ token 222 { 120 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 223 { 121 } false (wholesurface=\"made \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//UNORDERED GROUP (4)
QueryAnswer(false, "UNORDERED GROUP (4)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[Verse AS v1\n"
  "	[UnorderedGroup\n"
  "		[Token surface_stripped_lowcase='god' AND self > v1.self GET wholesurface]\n"
  "	]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ verse 23 { 8-17 } false  //  <  < [ token 113 { 11 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 24 { 18-45 } false  //  <  < [ token 140 { 38 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 25 { 46-56 } false  //  <  < [ token 149 { 47 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 26 { 57-73 } false  //  <  < [ token 160 { 58 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 169 { 67 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 27 { 74-95 } false  //  <  < [ token 177 { 75 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 28 { 96-118 } false  //  <  < [ token 199 { 97 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 29 { 119-144 } false  //  <  < [ token 222 { 120 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 30 { 145-161 } false  //  <  < [ token 248 { 146 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 31 { 162-186 } false  //  <  < [ token 265 { 163 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 32 { 187-210 } false  //  <  < [ token 290 { 188 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 307 { 205 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//UNORDERED GROUP (5)
QueryAnswer(false, "UNORDERED GROUP (5)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[Verse AS v1\n"
  "	[UnorderedGroup\n"
  "		[Token surface_stripped_lowcase='good']\n"
  "		[Token surface_stripped_lowcase='god' AND self > v1.self GET wholesurface]\n"
  "	]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ verse 26 { 57-73 } false  //  <  < [ token 167 { 65 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 160 { 58 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 169 { 67 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 32 { 187-210 } false  //  <  < [ token 312 { 210 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 290 { 188 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 307 { 205 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//IN (integer)
QueryAnswer(false, "IN (integer)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[Verse chapter IN (1) AND verse IN (3)\n"
  "    [Token GET wholesurface]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ verse 25 { 46-56 } false  //  <  < [ token 148 { 46 } false (wholesurface=\"And \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 149 { 47 } false (wholesurface=\"God \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 150 { 48 } false (wholesurface=\"said, \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 151 { 49 } false (wholesurface=\"Let \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 152 { 50 } false (wholesurface=\"there \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 153 { 51 } false (wholesurface=\"be \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 154 { 52 } false (wholesurface=\"light: \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 155 { 53 } false (wholesurface=\"and \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 156 { 54 } false (wholesurface=\"there \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 157 { 55 } false (wholesurface=\"was \") //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 158 { 56 } false (wholesurface=\"light. \") //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//IN (ENUM)
QueryAnswer(false, "IN (ENUM)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[title divtype IN (book)]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ title 19 { 1-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//IN (ID_D)
QueryAnswer(false, "IN (ID_D)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[title self IN (19)]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ title 19 { 1-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//IN (ENUM) (1)
QueryAnswer(false, "IN (ENUM) (1)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[title divtype IN (book)]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ title 19 { 1-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//IN (ENUM) (2)
QueryAnswer(false, "IN (ENUM) (2)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[title divtype IN (book, chapter)]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ title 19 { 1-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//IN (ID_D) (1)
QueryAnswer(false, "IN (ID_D) (1)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[title self IN (19)]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ title 19 { 1-7 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//IN (ID_D) (2)
QueryAnswer(false, "IN (ID_D) (2)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[token self IN (156, 158)]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ token 156 { 54 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " < [ token 158 { 56 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//IN (integer) (2)
QueryAnswer(false, "IN (integer) (2)",
  "SELECT ALL OBJECTS\n"
  "WHERE	  \n"
  "[Verse chapter IN (1) AND verse IN (3,5)\n"
  "    [Token FIRST]\n"
  "]\n"
  "GO\n"
  "",
  true, true,
  " //  <  < [ verse 25 { 46-56 } false  //  <  < [ token 148 { 46 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " < [ verse 27 { 74-95 } false  //  <  < [ token 176 { 74 } false  //  <  > \n"
  " ]\n"
  " > \n"
  " > \n"
  " ]\n"
  " > \n"
  " > \n"
  ""),


//STOP
QueryAnswer(false, "STOP",
  "",
  true, true,
  ""),
};
