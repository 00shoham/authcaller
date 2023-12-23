#include "base.h"

char defaultKey[] =
  {
  0xdb, 0xd2, 0x5a, 0x74, 0xc7, 0x44, 0x3b, 0x43,
  0xd2, 0x24, 0xcb, 0xce, 0x63, 0x4a, 0xe5, 0x4d,
  0x5d, 0x64, 0xd6, 0xcc, 0xcb, 0x97, 0x75, 0x2b,
  0x95, 0xa8, 0x52, 0x85, 0xe6, 0xed, 0x32, 0xc1
  };

void SetDefaults( _CONFIG* config )
  {
  memset( config, 0, sizeof(_CONFIG) );
  config->pinFolder = strdup( PIN_FOLDER );
  config->pinDigits = PIN_DIGITS;
  config->pinLifetimeSeconds = PIN_LIFETIME_SECONDS;

  memcpy( config->key, defaultKey, AES_KEYLEN );
  config->myCSS = strdup( DEFAULT_MY_CSS );
  config->userEnvVar = strdup( DEFAULT_USER_ENV_VAR );
  config->sessionCookieName = strdup( DEFAULT_ID_OF_AUTH_COOKIE );
  config->urlEnvVar = strdup( DEFAULT_REQUEST_URI_ENV_VAR );
  config->authServiceUrl = strdup( DEFAULT_AUTH_URL );
  }

void FreeConfig( _CONFIG* config )
  {
  if( config==NULL )
    return;

  FreeIfAllocated( &( config->myCSS) );

  FreeIfAllocated( &(config->pinFolder) );

  FreeIfAllocated( &(config->userEnvVar) );
  FreeIfAllocated( &(config->sessionCookieName) );
  FreeIfAllocated( &(config->urlEnvVar) );
  FreeIfAllocated( &(config->authServiceUrl) );

  free( config );
  }

void ProcessConfigLine( char* ptr, char* equalsChar, _CONFIG* config )
  {
  *equalsChar = 0;

  char* variable = TrimHead( ptr );
  TrimTail( variable );
  char* value = TrimHead( equalsChar+1 );
  TrimTail( value );

  if( NOTEMPTY( variable ) && NOTEMPTY( value ) )
    {
    if( strcasecmp( variable, "MY_CSS" )==0 )
      {
      FreeIfAllocated( &( config->myCSS) );
      config->myCSS = strdup( value );
      }
    else if( strcasecmp( variable, "USER_ENV_VARIABLE" )==0 )
      {
      FreeIfAllocated( &( config->userEnvVar ) );
      config->userEnvVar = strdup( value );
      }
    else if( strcasecmp( variable, "PIN_FOLDER" )==0 )
      {
      FreeIfAllocated( &( config->pinFolder) );
      config->pinFolder = strdup( value );
      }
    else if( strcasecmp( variable, "PIN_DIGITS" )==0 )
      {
      int n = atoi( value );
      if( n<PIN_MIN_DIGITS || n>PIN_MAX_DIGITS )
        Error( "Invalid number for PIN_DIGITS (%d-%d)", PIN_MIN_DIGITS, PIN_MAX_DIGITS );
      config->pinDigits = n;
      }
    else if( strcasecmp( variable, "PIN_LIFETIME_SECONDS" )==0 )
      {
      int n = atoi( value );
      if( n<PIN_MIN_LIFETIME || n>PIN_MAX_LIFETIME )
        Error( "Invalid number for PIN_LIFETIME_SECONDS (%d-%d)", PIN_MIN_LIFETIME, PIN_MAX_LIFETIME );
      config->pinLifetimeSeconds = n;
      }
    else if( strcasecmp( variable, "SESSION_COOKIE_NAME" )==0 )
      {
      FreeIfAllocated( &(config->sessionCookieName) );
      config->sessionCookieName = strdup( value );
      }
    else if( strcasecmp( variable, "AUTHENTICATION_SERVICE_URL" )==0 )
      {
      FreeIfAllocated( &(config->authServiceUrl) );
      config->authServiceUrl = strdup( value );
      }
    else if( strcasecmp( variable, "URL_ENV_VARIABLE" )==0 )
      {
      FreeIfAllocated( &(config->urlEnvVar) );
      config->urlEnvVar = strdup( value );
      }
    else if( strcasecmp( variable, "SESSION_COOKIE_ENCRYPTION_KEY" )==0 )
      {
      uint8_t binaryKey[100];
      memset( binaryKey, 0, sizeof(binaryKey) );
      UnescapeString( value, binaryKey, sizeof(binaryKey)-1 );
      memset( config->key, 0, AES_KEYLEN );
      memcpy( config->key, binaryKey, AES_KEYLEN );
      }
    else
      {
      /* append this variable to our linked list, for future expansion */
      /* do this always, so not here for just
         invalid commands:
         config->list = NewTagValue( variable, value, config->list, 1 );
      */
      }
    }
  }

void PrintConfig( FILE* f, _CONFIG* config )
  {
  if( f==NULL )
    {
    Error("Cannot print configuration to NULL file");
    }

  if( NOTEMPTY( config->myCSS )
      && strcmp( config->myCSS, DEFAULT_MY_CSS )!=0 )
    fprintf( f, "MY_CSS=%s\n", config->myCSS );

  if( NOTEMPTY( config->userEnvVar ) )
    fprintf( f, "USER_ENV_VARIABLE=%s\n", config->userEnvVar );

  if( memcmp( config->key, defaultKey, AES_KEYLEN )!=0 )
    {
    char key_ascii[100];
    fprintf( f, "SESSION_COOKIE_ENCRYPTION_KEY=%s\n", EscapeString( config->key, AES_KEYLEN, key_ascii, sizeof( key_ascii ) ) );
    }

  if( NOTEMPTY( config->sessionCookieName )
      && strcmp( config->sessionCookieName, DEFAULT_ID_OF_AUTH_COOKIE )!=0 )
    {
    fprintf( f, "SESSION_COOKIE_NAME=%s\n", config->sessionCookieName );
    }

  if( NOTEMPTY( config->authServiceUrl )
      && strcmp( config->authServiceUrl, DEFAULT_AUTH_URL )!=0 )
    {
    fprintf( f, "SESSION_COOKIE_NAME=%s\n", config->authServiceUrl );
    }

  if( NOTEMPTY( config->urlEnvVar )
      && strcmp( config->urlEnvVar, DEFAULT_REQUEST_URI_ENV_VAR )!=0 )
    {
    fprintf( f, "URL_ENV_VARIABLE=%s\n", config->urlEnvVar );
    }

  if( NOTEMPTY( config->pinFolder ) &&
      strcmp( config->pinFolder, PIN_FOLDER )!=0 )
    {
    fprintf( f, "PIN_FOLDER=%s\n", config->pinFolder );
    }

  if( config->pinDigits != PIN_DIGITS )
    {
    fprintf( f, "PIN_DIGITS=%d\n", config->pinDigits );
    }

  if( config->pinLifetimeSeconds != PIN_LIFETIME_SECONDS )
    {
    fprintf( f, "PIN_LIFETIME_SECONDS=%d\n", config->pinLifetimeSeconds );
    }
  }

void ReadConfig( _CONFIG* config, char* filePath )
  {
  char folder[BUFLEN];
  folder[0] = 0;
  (void)GetFolderFromPath( filePath, folder, sizeof( folder )-1 );

  // Notice( "Config is being read from folder [%s]", folder );

  if( EMPTY( filePath ) )
    {
    Error( "Cannot read configuration file with empty/NULL name");
    }

  FILE* f = fopen( filePath, "r" );
  if( f==NULL )
    {
    Error( "Failed to open configuration file %s", filePath );
    }

  char buf[BUFLEN];
  while( fgets(buf, sizeof(buf)-1, f )==buf )
    {
    char* ptr = TrimHead( buf );
    TrimTail( ptr );

    if( *ptr==0 )
      {
      continue;
      }

    /* printf("Processing [%s]\n", ptr ); */
    char* equalsChar = NULL;
    for( char* eolc = ptr; *eolc!=0; ++eolc )
      {
      if( equalsChar==NULL && *eolc == '=' )
        {
        equalsChar = eolc;
        }

      if( *eolc == '\r' || *eolc == '\n' )
        {
        *eolc = 0;
        break;
        }
      }

    if( *ptr!=0 && equalsChar!=NULL && equalsChar>ptr )
      {
      ProcessConfigLine( ptr, equalsChar, config );
      }
    }

  fclose( f );
  }

void ValidateConfig( _CONFIG* config )
  {
  if( config==NULL )
    Error( "Cannot validate a NULL configuration" );

  if( EMPTY( config->myCSS ) )
    Error( "MY_CSS must be set (or left as default) in config" );

  if( EMPTY( config->userEnvVar ) )
    Error( "USER_ENV_VARIABLE must be set in config file" );

  if( EMPTY( config->pinFolder ) )
    Error( "PIN_FOLDER must be set in config file" );

  if( DirExists( config->pinFolder )!=0 )
    Error( "Folder [%s] does not exist or cannot be opened", config->pinFolder );

  if( config->pinDigits < PIN_MIN_DIGITS
      || config->pinDigits > PIN_MAX_DIGITS )
    Error( "PIN_DIGITS must be from %d to %d", PIN_MIN_DIGITS, PIN_MAX_DIGITS );

  if( config->pinLifetimeSeconds < PIN_MIN_LIFETIME
      || config->pinLifetimeSeconds > PIN_MAX_LIFETIME )
    Error( "PIN_LIFETIME_SECONDS must be from %d to %d", PIN_MIN_LIFETIME, PIN_MAX_LIFETIME );

  }

