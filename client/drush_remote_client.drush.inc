<?php
/**
 * @file
 * Drush Remote Client module drush integration.
 */


// Webservice path on the server.
define('DRUSH_REMOTE_WS_PATH', 'drush-remote/drush/execute');

/**
 * Implements hook_drush_command().
 */
function drush_remote_client_drush_command() {
  $items['remote'] = array(
    'description' => 'Execute a drush command remotely.',
    'arguments' => array(
      'command' => 'A drush command to execute on the server.',
    ),
    'examples' => array(
      'drush remote enable views' => 'Enable module views on a remote site.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  return $items;
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_drush_remote_client_remote() {
  $authentication = NULL;
  $formatter = new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON);
  $client = new HttpClient($authentication, $formatter);

  global $base_url;
  // TODO remote / ?
  // TODO use --remote-host instead of --uri ?
  $request = new HttpClientRequest("$base_url/?q=" . DRUSH_REMOTE_WS_PATH, array(
    'method' => 'POST',
    'parameters' => NULL,
    'data' => implode(' ', func_get_args()),
  ));
  try {
    $response = $client->execute($request);
drush_print_r($response);
  }
  catch (HttpClientException $e) {
    // TODO
drush_print_r($e);
  }
}
