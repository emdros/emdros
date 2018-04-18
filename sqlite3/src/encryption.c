/*
** Key an encrypted SQLite database.  This does nothing.
**
** The code here is implemented by Ulrik Petersen.  Another version
** is available from D. Richard Hipp, the author of SQLite, which
** actually does enable encryption.
*/
int sqlite3_key(sqlite3 *pDB,      /** DB to alter. */
               const void* pKey,  /** The key. */
               int nKey)          /** Key's length. */
{
	(void)(pDB);
	(void)(pKey);
	(void)(nKey);
	return SQLITE_OK;
}


/*
 * This code is by Ulrik Sandborg-Petersen.
 * Dr. Hipp, the creator of SQLite, has real encryption.
 */
int sqlite3_open_encrypted(
  const char *zFilename,   /* Name of the encrypted database */
  const void *pKey,        /* Pointer to the key */
  int nKey,                /* Number of bytes in the key */
  sqlite3 **ppDB
)
{
	(void)(pKey);
	(void)(nKey);
        return sqlite3_open(zFilename, ppDB);
}

