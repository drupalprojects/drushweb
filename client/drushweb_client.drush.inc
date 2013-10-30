<?php
/**
 * @file
 * Drush webservice Client module drush integration.
 */


// Webservice path on the server.
define('DRUSHWEB_PATH', 'drushweb/drush/execute');

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

  // TODO make this dynamic
  $consumer_key = 'y4ZYMrw8VNMDsnkibMxwu8ZybS4GPzHV';
  $consumer_secret = 'JHjnXtfpLxrPCVwnjyn8dCQmsxNmgfEZ';

  $sign = new OAuthSignatureMethod_HMAC('SHA1');
  $auth = new HttpClientOAuth($consumer_key, NULL, $sign);
  $client = http_client($auth);

  global $base_url;
  $commands = drush_get_command();
  $commands = $commands['arguments'];
  // TODO support all options ?
  if (drush_get_context('DRUSH_AFFIRMATIVE')) {
    $commands[] = '--yes';
  }
  try {
    // TODO webservice / ?
    $response = $client->post(
      "$base_url/?q=" . DRUSHWEB_PATH,
      array(
        'parameters' => NULL,
        'data' => implode(' ', $commands),
      )
    );
  }
  catch (HttpClientException $e) {
    $msg = dt('Failed to communicate with the server: !msg', array('!msg' => $e->getMessage()));
    return drush_user_abort($msg);
  }

  // TODO execute() might not return an array
  // TODO the response might not be splittable with \n
  foreach ($response as $lines) {
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

// TODO HTTP headers = do not cache response
// TODO check PHP prerequisites on client (see #1871498) - or let http_client take care or this
// TODO oauth config -> request a token
