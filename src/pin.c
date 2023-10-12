#include "base.h"

int WritePin( _CONFIG* config, char* pin, char* user )
  {
  if( config==NULL || EMPTY( config->pinFolder ) )
    Error( "Cannot write PIN without config" );

  if( EMPTY( pin ) )
    Error( "Cannot write NULL PIN" );

  if( EMPTY( user ) )
    Error( "Cannot write PIN with no user" );

  char* pinPath = MakeFullPath( config->pinFolder, pin );
  if( EMPTY( pinPath ) )
    Error( "Failed to create path for PIN file" );

  FILE* f = fopen( pinPath, "w" );
  if( f==NULL )
    Error( "Failed to open PIN file for writing" );

  fputs( user, f );
  fputs( "\n", f );

  time_t tNow = time( NULL );
  char timeBuf[100];
  snprintf( timeBuf, sizeof(timeBuf)-10, "%lx\n", (long)tNow );
  fputs( timeBuf, f );

  fclose( f );
  free( pinPath );

  return 0;
  }

char* GetUser( _CONFIG* config )
  {
  if( config==NULL || EMPTY( config->userEnvVar ) )
    Error( "Cannot GetUser without config or userEnvVar" );

  char* userName = getenv( config->userEnvVar );
  if( EMPTY( userName ) )
    Error( "Cannot determine authenticated user ID" );

  return strdup( userName );
  }

extern int randSeeded;

char* GeneratePin( _CONFIG* config, char* whoami )
  {
  if( config==NULL || EMPTY( config->pinFolder ) )
    Error( "Cannot write PIN without config" );

  int nBytes = config->pinDigits;
  char* pin = (char*)SafeCalloc( nBytes+1, sizeof(char), "PIN" );

  if( randSeeded==0 )
    {
    srand48( time( NULL ) );
    randSeeded = 1;
    }

  for( int i=0; i<nBytes; ++i )
    {
    int c = lrand48() % 10;
    pin[i] = c + '0';
    }

  if( WritePin( config, pin, whoami )!=0 )
    Error( "Failed to write PIN!" );

  return pin;
  }

int ValidatePin( _CONFIG* config, char* pin, char** userIdString )
  {
  if( config==NULL || EMPTY( config->pinFolder ) )
    Error( "Cannot validate PIN without config" );

  if( EMPTY( pin ) )
    Error( "Cannot validate NULL PIN" );

  if( userIdString==NULL )
    Error( "Cannot validate PIN with no return user string" );

  char* pinPath = MakeFullPath( config->pinFolder, pin );
  if( EMPTY( pinPath ) )
    Error( "Failed to create path for PIN file" );

  FILE* f = fopen( pinPath, "r" );
  if( f==NULL )
    {
    Warning( "Invalid PIN" );
    return -1;
    }

  char userStr[100];
  char timeStr[100];

  if( fgets( userStr, sizeof(userStr)-1, f )!=userStr )
    {
    Warning( "No USER in PIN file" );
    fclose( f );
    free( pinPath );
    return -2;
    }

  if( fgets( timeStr, sizeof(timeStr)-1, f )!=timeStr )
    {
    Warning( "No TIME in PIN file" );
    fclose( f );
    free( pinPath );
    return -3;
    }

  fclose( f );
  free( pinPath );

  char* userTrim = TrimHead( userStr );
  (void)TrimTail( userTrim );

  char* timeTrim = TrimHead( timeStr );
  (void)TrimTail( timeTrim );

  if( EMPTY( userTrim ) )
    {
    Warning( "Empty USER in PIN file" );
    return -4;
    }

  if( EMPTY( timeTrim ) )
    {
    Warning( "Empty TIME in PIN file" );
    return -5;
    }

  long lt = 0;
  if( sscanf( timeTrim, "%lx", &lt )!=1 )
    {
    Warning( "Invalid TIME in PIN file" );
    return -6;
    }

  time_t tNow = time( NULL );
  long age = (long)tNow - lt;
  if( age > config->pinLifetimeSeconds )
    {
    Warning( "TIME in PIN file too old" );
    return -7;
    }

  *userIdString = strdup( userTrim );
  return 0;
  }


void CleanupPinFolder( _CONFIG* config )
  {
  if( config==NULL || EMPTY(config->pinFolder) )
    Error( "Cannot cleanup non-existent folder" );

  DIR* d = opendir( config->pinFolder );
  if( d==NULL )
    Error( "Cannot cleanup non-openable folder" );

  struct dirent * de;
  while( (de=readdir( d ))!=NULL )
    {
    if( NOTEMPTY( de->d_name ) )
      {
      long age = GetFileAge( config->pinFolder, de->d_name );
      if( age > config->pinLifetimeSeconds )
        {
        char* pinPath = MakeFullPath( config->pinFolder, de->d_name );
        unlink( pinPath );
        free( pinPath );
        }
      }
    }

  closedir( d );
  }

void CleanupPinFolderInDaemon( _CONFIG* config )
  {
  pid_t childProcess = LaunchDaemon( 1 );
  if( childProcess<0 )
    Error( "Failed to launch daemon for folder cleanup" );
  else if( childProcess>0 )
    Notice( "Launched the cleanup daemon - pid=%ld", (long)childProcess );
  else
    {
    CleanupPinFolder( config );
    exit(0);
    }
  }
