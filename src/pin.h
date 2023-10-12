#ifndef _INCLUDE_PIN
#define _INCLUDE_PIN

int WritePin( _CONFIG* config, char* pin, char* user );
char* GetUser( _CONFIG* conf );
char* GeneratePin( _CONFIG* config, char* whoami );
int ValidatePin( _CONFIG* config, char* pin, char** userIdString );
void CleanupPinFolder( _CONFIG* config );
void CleanupPinFolderInDaemon( _CONFIG* config );

#endif
