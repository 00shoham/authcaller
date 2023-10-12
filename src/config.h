#ifndef _INCLUDE_CONFIG
#define _INCLUDE_CONFIG

#define CONFIGDIR "/usr/local/etc/"
#define CONFIGFILE "authcaller.ini"
#define PIN_FOLDER "/var/authcaller/pins"
#define PIN_DIGITS 6
#define PIN_MIN_DIGITS 3
#define PIN_MAX_DIGITS 10
#define PIN_LIFETIME_SECONDS 120
#define PIN_MIN_LIFETIME 30
#define PIN_MAX_LIFETIME 600

typedef struct _config
  {
  char* userEnvVar;
  char* pinFolder;
  int pinDigits;
  int pinLifetimeSeconds;
  } _CONFIG;

void SetDefaults( _CONFIG* config );
void ReadConfig( _CONFIG* config, char* filePath );
void PrintConfig( FILE* f, _CONFIG* config );
void FreeConfig( _CONFIG* config );
void ValidateConfig( _CONFIG* config );

#endif
