#include "base.h"

void SetDefaults( _CONFIG* config )
  {
  memset( config, 0, sizeof(_CONFIG) );
  config->pinFolder = strdup( PIN_FOLDER );
  config->pinDigits = PIN_DIGITS;
  config->pinLifetimeSeconds = PIN_LIFETIME_SECONDS;
  }

void FreeConfig( _CONFIG* config )
  {
  if( config==NULL )
    return;

  FreeIfAllocated( &(config->userEnvVar) );
  FreeIfAllocated( &(config->pinFolder) );

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
    if( strcasecmp( variable, "USER_ENV_VARIABLE" )==0 )
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
    }
  }

void PrintConfig( FILE* f, _CONFIG* config )
  {
  if( f==NULL )
    {
    Error("Cannot print configuration to NULL file");
    }

  if( NOTEMPTY( config->userEnvVar ) )
    {
    fprintf( f, "USER_ENV_VARIABLE=%s\n", config->userEnvVar );
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

