import sys
sys.path += ['/usr/local/lib/emdros', '/usr/lib/emdros']

import EmdrosPy

env = EmdrosPy.EmdrosEnv(EmdrosPy.kOKConsole, EmdrosPy.kCSISO_8859_1, "localhost", "emdf", "changeme", "emdf", EmdrosPy.kSQLite3)

env.out("string\n")

som = EmdrosPy.SetOfMonads()
som.add(1,5)
som.add(7,10)

env.out(som)
env.out("\n")

# Create database
bCompilerResult = False
bResult = env.executeString("CREATE DATABASE EMdF5 GO", bCompilerResult, True, True)
print "Query returned: no DB errors: ", bResult[0], " no compiler errors: ", bResult[1]


bResult = env.executeString("DROP DATABASE EMdF5 GO", bCompilerResult, True, True)
print "Query returned: no DB errors: ", bResult[0], " no compiler errors: ", bResult[1]

