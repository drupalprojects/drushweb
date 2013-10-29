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

  // List args + options and remove bin name + 'remote'.
  $argv = drush_get_context('argv');
  unset($argv[0]);
  if ($key = array_search('remote', $argv)) {
    unset($argv[$key]);
  }

  global $base_url;
  // TODO remote / ?
  $request = new HttpClientRequest("$base_url/?q=" . DRUSH_REMOTE_WS_PATH, array(
    'method' => 'POST',
    'parameters' => NULL,
    'data' => implode(' ', $argv),
  ));
  try {
    $response = $client->execute($request);
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
