#include "base.h"

#define SUCCESS "success"
#define ERROR "error"

#define DEBUG 1

_TAG_VALUE* SendShortResponse( int code, char* result, _TAG_VALUE* response )
  {
  char responseBuf[BUFLEN];
  response = NewTagValueInt( "code", code, response, 1 );
  response = NewTagValue( "result", result, response, 1 );
  ListToJSON( response, responseBuf, sizeof(responseBuf)-1 );
  fputs( responseBuf, stdout );
  fputs( "\r\n", stdout );
  return response;
  }

void SimpleError( char* msg )
  {
  printf( "<html><body><b>ERROR: %s\n</b></body></html>\n",
          msg );
  }

void ApiPinGenerate( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  if( conf==NULL )
    APIError( methodName, -999, "No config" );
  if( EMPTY( userID ) )
    APIError( methodName, -998, "No userID" );
  if( StringIsAnIdentifier( userID )!=0 )
    APIError( methodName, -997, "userID must be an identifier" );

  char* pin = GeneratePin( conf, userID );
  if( EMPTY( pin ) )
    APIError( methodName, -996, "Failed to generate or store PIN" );

  printf( "{ \"PIN\":\"%s\", \"result\":\"%s\", \"code\": 0 }\n", pin, SUCCESS );
  }

void ApiPinValidate( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  char* pin = GetTagValue( args, "PIN" );

  if( NOTEMPTY( pin ) )
    {
    char* pinBelongsTo = NULL;
    int err = ValidatePin( conf, pin, &pinBelongsTo );
    if( err )
      APIError( methodName, -100, "PIN %s is not valid (%d)", pin, err );

    if( EMPTY( pinBelongsTo ) )
      APIError( methodName, -101, "PIN found but does not belong to anyone" );

    if( StringIsAnIdentifier( pinBelongsTo )!=0 )
      APIError( methodName, -102, "PIN owner user must be an identifier" );

    printf( "{ \"PIN-OWNER\":\"%s\", \"result\":\"%s\", \"code\": 0 }\n", pinBelongsTo, SUCCESS );
    }
  else
    APIError( methodName, -103, "Cannot validate empty PIN" );
  }

void ApiGetUserID( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  if( NOTEMPTY( userID ) )
    printf( "{ \"USER\":\"%s\", \"result\":\"%s\", \"code\": 0 }\n", userID, SUCCESS );
  else
    APIError( methodName, -100, "User not authenticated." );
  }

void GetLogoutURL( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  if( conf==NULL )
    APIError( methodName, -1, "No configuration" );
  if( EMPTY( conf->authServiceUrl ) )
    APIError( methodName, -2, "No AUTH_SERVICE_URL not set in configuration" );

  char url[BUFLEN];
  snprintf( url, sizeof(url)-1, "%s?LOGOUT", conf->authServiceUrl );

  printf( "{ \"URL\":\"%s\", \"result\":\"%s\", \"code\": 0 }\n", url, SUCCESS );
  }

typedef struct fnRecord
  {
  char* topic;
  char* action;
  void (*function)( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args );
  } _FN_RECORD;

_FN_RECORD methods[] =
  {
    { "pin", "generate", ApiPinGenerate },
    { "pin", "validate", ApiPinValidate },
    { "user", "identity", ApiGetUserID },
    { "logout", "get-url", GetLogoutURL },
    { "NULL", "NULL", NULL }
  };

void CallAPIFunction( _CONFIG* conf, char* userID, char* method )
  {
  char* ptr = NULL;
  char* topic = strtok_r( method, "&/", &ptr );
  char* action = strtok_r( NULL, "&/", &ptr );

  if( StringIsAnIdentifier( userID )!=0 )
    APIError( method, -99, "API user (string) must be an identifier" );

  if( EMPTY( topic ) || EMPTY( action ) )
    APIError( method, -4, "API calls must specify a topic and action " );

#if 0 /* there is no restriction on what authetnicated user can use the API for this program */
  /* is this a known user?  If not, API error */
  if( conf->users!=NULL
      && FindTagValue( conf->users, userID )==NULL )
    APIError( method, -5, "API call using unknown user ID" );
#endif

  char methodNameBuf[BUFLEN];
  snprintf( methodNameBuf, sizeof(methodNameBuf)-1, "%s/%s", topic, action );

  char inputBuf[BUFLEN];
  if( fgets( inputBuf, sizeof(inputBuf)-1, stdin ) != inputBuf )
    inputBuf[0] = 0;

  _TAG_VALUE* args = ParseJSON( inputBuf );
 
  /* cleanup old PIN files.. */
  CleanupPinFolder( conf );
  /* CleanupPinFolderInDaemon( conf ); */

  int gotOne = 0;
  for( _FN_RECORD* fn=methods; fn->topic!=NULL; ++fn )
    {
    if( NOTEMPTY( fn->topic )
        && NOTEMPTY( fn->action )
        && fn->function!=NULL
        && strcasecmp( fn->topic, topic )==0
        && strcasecmp( fn->action, action )==0 )
      {
      (fn->function)( conf, userID, methodNameBuf, args );
      gotOne = 1;
      break;
      }
    }

  if( ! gotOne )
    APIError( method, -5, "Invalid function and/or topic. (%s/%s)", topic, action );

  FreeTagValue( args );
  }

