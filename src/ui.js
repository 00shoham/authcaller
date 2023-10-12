var baseURL = "/cgi-bin/authcaller?api";

var messageTimeout = 5*1000;

function DisplayError( msg )
  {
  var errDiv = document.getElementById("error");
  if( typeof errDiv != "undefined" )
    {
    errDiv.innerHTML = msg;
    errDiv.style.display = "block";
    setTimeout(function(){ errDiv.style.display = "none"; }, messageTimeout);
    }
  else
    alert( msg );
  }

function CallAPIFunction( topic, action, args )
  {
  var method = "/" + topic + "/" + action;
  var xhttp = new XMLHttpRequest();
  xhttp.open( "POST", baseURL + method, false );
  xhttp.send( JSON.stringify(args) );
  var responseText = xhttp.responseText; 
  var responseObj;
  try
    {
    responseObj = JSON.parse( responseText );
    }
  catch( err )
    {
    DisplayError( err.message );
    }

  var errmsg = "An error has occurred.  No details available.";

  if( typeof( responseObj ) == "undefined"
      || typeof responseObj.code == "undefined"
      || typeof responseObj.result == "undefined" )
    {
    DisplayError( errmsg );
    }
  else if( responseObj.code !=0 )
    {
    errmsg = responseObj.result;
    DisplayError( errmsg );
    }

  return responseObj;
  }

// used to test API functions.
function RunSimple()
  {
  var args = { };
  var responseObj = CallAPIFunction( "pin", "generate", args );

  if( typeof responseObj != "undefined"
      && typeof responseObj.code != "undefined"
      && responseObj.code==0 )
    {
    var htmlObj = document.getElementById("content-a");
    if( typeof htmlObj != "undefined" )
      {
      htmlObj.innerHTML = JSON.stringify( responseObj );
      }
    }
  else
    {
    DisplayError( "Bad API response" )
    }

  }

function ErrorParser( responseObj, action, detail )
  {
  if( typeof responseObj == "undefined"
      || typeof responseObj.code == "undefined"
      || typeof responseObj.result == "undefined" )
    return "Calling " + action + " failed for " + detail
           + ": undefined return code.";

  if( responseObj.code!=0 )
    return "Calling " + action + " failed for " + detail
           + ": " + responseObj.code
           + " - " + responseObj.result
           + ".";

  return "";
  }


function ReportResults( msg )
  {
  var errDiv = document.getElementById("error");
  if( typeof errDiv != "undefined" )
    {
    errDiv.innerHTML = msg;
    errDiv.style.display = "block";
    setTimeout(function(){ errDiv.style.display = "none"; }, messageTimeout);
    }
  }

function Generate()
  {
  var genDiv = document.getElementById("generated");
  if( genDiv )
    {
    var args = { };
    var responseObj = CallAPIFunction( "pin", "generate", args );
    if( typeof responseObj != "undefined"
        && typeof responseObj.code != "undefined"
        && responseObj.code==0 )
      {
      if( typeof responseObj["PIN"] != "undefined" )
        {
        genDiv.innerHTML = "Your PIN is " + responseObj["PIN"];
        }
      else
        {
        genDiv.innerHTML = "ERROR with responseObj - " + JSON.stringify( responseObj );
        }
      }
    else
      DisplayError( "Invalid API response structure" );
    }
  else
    alert( "Malformed HTML - where do I stick the result?" );
  }

function Validate()
  {
  var resDiv = document.getElementById("result");

  if( resDiv )
    {
    pinElement = document.getElementById( "pin" );
    pin = pinElement.value;

    var args = { "PIN":pin };
    var responseObj = CallAPIFunction( "pin", "validate", args );

    if( typeof responseObj != "undefined"
        && typeof responseObj.code != "undefined" )
      {
      if( responseObj.code == 0 )
        {
        if( typeof responseObj["PIN-OWNER"] != "undefined" )
          {
          var msg = "PIN " + pin + " belongs to " + responseObj["PIN-OWNER"] + ".";
          resDiv.innerHTML = msg;
          DisplayError( msg );
          }
        else
          {
          resDiv.innerHTML = "PIN " + pin + " has an unknown owner - " + JSON.stringify( responseObj ) + ".";
          }
        }
      else
        DisplayError( "Invalid PIN - possibly timed out." );
      }
    else
      {
      DisplayError( "Invalid API response structure." );
      }
    }

  }

var doneInitialRefresh = 0;

function Refresh()
  {
  /* console.log( "Refresh()" ); */

  if( doneInitialRefresh )
    return;

  doneInitialRefresh = 1;

  var htmlContainer = document.getElementById( "content" );
  if( typeof htmlContainer == "undefined" )
    {
    DisplayError( "Missing HTML container - content" );
    return;
    }

  var errorContainer = document.getElementById( "error" );
  if( typeof errorContainer == "undefined" )
    {
    DisplayError( "Missing HTML container - error" );
    return;
    }
  else
    errorContainer.innerHTML = "&nbsp;";

  content = "";
  content += "<h1>Mutual Authentication</h1>\n";
  content += "<p>This application allows two people in the same organization to\n";
  content += "   verify one another's identity after both sign into this UI.</p>\n";
  content += "<table class='maintab'>\n";
  content += "  <tr>\n";
  content += "    <td class='head'>\n";
  content += "      <b>Generate</b>\n";
  content += "    </td>\n";
  content += "    <td class='head'>\n";
  content += "      <b>Validate</b>\n";
  content += "    </td>\n";
  content += "  </tr>\n";
  content += "  <tr>\n";
  content += "    <td class='input'>\n";
  content += "      <a class='generate' href='#' onclick='Generate();'>Generate PIN</a>\n";
  content += "      <br/>\n";
  content += "      <p class='generated' id='generated'></p>\n";
  content += "    </td>\n";
  content += "    <td class='input'>\n";
  content += "      <form id='pin-input' onsubmit='return false;' action='' method='get'>\n";
  content += "        <label for='pin'/>PIN:</label>\n";
  content += "        <input type='text' name='pin' id='pin' size='10' maxlength='20'/>\n";
  content += "        <a class='validate' href='#' onclick='Validate();'>Check PIN</a>\n";
  content += "      </form>\n";
  content += "      <br/>\n";
  content += "      <p class='result' id='result'></p>\n";
  content += "    </td>\n";
  content += "  </tr>\n";
  content += "  <tr>\n";
  content += "    <td class='foot'>\n";
  content += "      <p>Create a PIN that can be shared with someone else to prove to them that it's really you calling or texting them!</p>\n";
  content += "    </td>\n";
  content += "    <td class='foot'>\n";
  content += "      <p>Check a PIN that someone else gave you, to see who it really is!</p>\n";
  content += "    </td>\n";
  content += "  </tr>\n";
  content += "</table>\n";

  htmlContainer.innerHTML = content;

  var form = document.getElementById('pin-input');
  if( form.attachEvent )
    form.attachEvent( "submit", Validate );
  else
    form.addEventListener( "submit", Validate );

  var pinInput = document.getElementById('pin');
  if( pinInput )
    pinInput.focus();
  }


function InitializeUI()
  {
  Refresh();
  }

