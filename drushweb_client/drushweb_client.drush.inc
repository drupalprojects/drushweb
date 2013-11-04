<?php
/**
 * @file
 * Drush webservice Client module drush integration.
 */


// Server endpoint path.
define('DRUSHWEB_ENDPOINT_PATH', 'drushweb/drush/execute');

/**
 * Implements hook_drush_command().
 */
function drushweb_client_drush_command() {
  $items['webservice'] = array(
    'description' => 'Execute a drush command on a site running Drush webservice Server.',
    'arguments' => array(
      'command' => 'A drush command to execute on the server.',
    ),
    'examples' => array(
      'drush webservice enable views' => 'Enable module views on a site.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('web'),
  );
  return $items;
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_drushweb_client_webservice() {
  $cmd = drush_get_command();
  $cmd = $cmd['arguments'];
  if (drush_get_context('DRUSH_AFFIRMATIVE')) {// TODO support all options ? --verbose etc
    $cmd[] = '--yes';
  }
  $cmd = implode(' ', $cmd);

  $client = _drushweb_client_get_client();

  global $base_url;
  // TODO / ?
  $request = new HttpClientRequest($base_url . '/', array(
    'method' => 'POST',
    'data' => $cmd,
    'parameters' => array('q' => DRUSHWEB_ENDPOINT_PATH),
  ));
  _drushweb_client_process_request($client, $request);
}

/**
 * Send and process a request to the server.
 *
 * @param $client
 *   HttpClient object.
 * @param $request
 *   HttpClientRequest object.
 */
function _drushweb_client_process_request($client, $request) {
  try {
    $body = $client->execute($request);
    _drushweb_client_display_body($body);
  }
  catch (HttpClientException $e) {
    switch ($e->getCode()) {
      // TODO support 3xx codes
      case 401:
        // TODO make creds storable somewhere
        // TODO include realm 401 ?
        // TODO we support only basic http authentication
        // TODO support password occultation
        drush_print($e->getMessage());
        $user = 'admin';//TODO drush_prompt(dt('username'));
        $pass = 'abcd1234';//TODO drush_prompt(dt('password'));
        $client_auth = new HttpClient(NULL, new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON));
        global $base_url;
        $request_auth = new HttpClientRequest($base_url . '/', array(
          'method' => 'POST',
          'data' => 'my data',
          'parameters' => array('q' => DRUSHWEB_ENDPOINT_PATH),
        ));
        $request_auth->setHeader('Authorization', 'Basic ' . base64_encode("$user:$pass"));
        _drushweb_client_process_request($client_auth, $request_auth);
        break;
      default:
        $msg = dt('Failed to communicate with the server: !msg', array('!msg' => $e->getMessage()));
        return drush_user_abort($msg);
    }
  }
}

/**
 * Get HttpClient object.
 *
 * @return
 *   HttpClient object.
 */
function _drushweb_client_get_client() {
  // TODO make this dynamic
  $consumer_key = 'y4ZYMrw8VNMDsnkibMxwu8ZybS4GPzHV';
  $consumer_secret = 'JHjnXtfpLxrPCVwnjyn8dCQmsxNmgfEZ';

  $consumer = new DrupalOAuthConsumer($consumer_key, $consumer_secret);
  $sign = DrupalOAuthClient::signatureMethod();
  $auth = new HttpClientOAuth($consumer, NULL, $sign, TRUE, TRUE);
  $formatter = new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON);
  return http_client($auth, $formatter);
}

/**
 * Format and display the body of the server response.
 *
 * @param $body
 *   Body of Http response.
 */
function _drushweb_client_display_body($body) {
  // TODO execute() might not return an array
  // TODO the response might not be splittable with \n
  foreach ($body as $lines) {
    foreach (explode("\n", $lines) as $line) {
      $entry = json_decode($line);
      if (is_object($entry)) {
        drush_log($entry->message, $entry->type, $entry->error);
      }
      else {
        drush_print($line);
      }
    }
  }
}

// TODO check PHP prerequisites on client (see #1871498) - or let http_client take care or this
// TODO msg if consumer key invalid
// TODO check drush minimum version
// TODO does it work when drushweb_client is disabled ?
