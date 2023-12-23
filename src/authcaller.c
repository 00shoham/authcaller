#include "base.h"

void DefaultPage()
  {
  printf("Content-Type: text/html\r\n\r\n");

  printf("<html><body><p>API request must be provided.</p></body></html>\n");
  }

extern char **environ;

void Usage( char* cmd )
  {
  printf("USAGE: %s [-h] [-generate USERID] [-validate PIN]\n", cmd );
  printf("       example: %s -generate BOB\n", cmd );
  printf("       example: %s -validate 12345\n", cmd );
  }

int main( int argc, char** argv )
  {
  char* confPath = MakeFullPath( CONFIG_DIR, CONFIG_FILE );
  _CONFIG* conf = (_CONFIG*)calloc( 1, sizeof( _CONFIG ) );
  if( conf==NULL ) Error( "Cannot allocate CONFIG object" );

  SetDefaults( conf );
  ReadConfig( conf, confPath );
  ValidateConfig( conf );

  char* q = getenv( "QUERY_STRING" );

  if( EMPTY(q) && argc>1 )
    { /* CLI */
    char* userID = NULL;
    char* pin = NULL;

    for( int i=1; i<argc; ++i )
      {
      if( strcmp( argv[i], "-h" )==0 )
        {
        Usage( argv[0] );
        exit(0);
        }
      else if( strcmp( argv[i], "-generate" )==0 && i+1<argc )
        {
        ++i;
        userID = argv[i];
        }
      else if( strcmp( argv[i], "-validate" )==0 && i+1<argc )
        {
        ++i;
        pin = argv[i];
        }
      else
        {
        fprintf( stderr, "Invalid option: [%s]\n", argv[i] );
        Usage( argv[0] );
        exit(-1);
        }
      }

    if( EMPTY( pin ) && EMPTY( userID ) )
      Error( "You must specify either -generate USER or -validate PIN" );
    if( NOTEMPTY( pin ) && NOTEMPTY( userID ) )
      Error( "You must specify *either* -generate USER or -validate PIN" );
    if( NOTEMPTY( pin ) )
      {
      char* pinUser = NULL;
      int err = ValidatePin( conf, pin, &pinUser );
      if( err )
        Error( "Failed to validate PIN %s", pin );
      if( EMPTY( pinUser ) )
        Error( "PIN %s validates but no user", pin );
      printf( "User is %s\n", pinUser );
      free( pinUser );
      }
    if( NOTEMPTY( userID ) )
      {
      char* generatedPin = GeneratePin( conf, userID );
      if( EMPTY( generatedPin ) )
        Error( "Failed to generate PIN" );
      int err = WritePin( conf, generatedPin, userID );
      if( err )
        Error( "Failed to write PIN" );
      printf( "Pin is %s\n", generatedPin );
      }
    }
  else
    {

    if( EMPTY( q ) )
      { /* send the browser to a likely URL for the app UI */
      printf( "Location: /authcaller/ui.html\r\n\r\n");
    
      DefaultPage();
      exit(0);
      }
    else
      { /* URL received - CGI action */
      if( ( strncasecmp( q, "api&", 4 )==0 || strncasecmp( q, "api/", 4 )==0 )
          && q[4]!=0 )
        {
        inCGI = 2;
        /* char* whoAmI = ExtractUserIDOrDie( cm_api, conf->userEnvVar ); */
        char* whoAmI = ExtractUserIDOrDieEx( cm_api,
                                             conf->userEnvVar, conf->sessionCookieName,
                                             conf->urlEnvVar, conf->authServiceUrl,
                                             conf->key,
                                             conf->myCSS );

        fputs( "Content-Type: application/json\r\n\r\n", stdout );

        CallAPIFunction( conf, whoAmI, q+4 );
        fflush(stdout);
        FreeConfig( conf );
        exit(0);
        }
      else
        {
        printf( "APIError: Invalid API HTTP GET prefix\r\n");
        printf( "Location: /authcaller/ui.html\r\n\r\n");
      
        DefaultPage();
        exit(0);
        }
      } /* URL had args */
    }

  return 0;
  }
