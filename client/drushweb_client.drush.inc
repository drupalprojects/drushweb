<?php
/**
 * @file
 * Drush Webservice Client module drush integration.
 */


// Webservice path on the server.
define('DRUSHWEB_PATH', 'drushweb/drush/execute');

/**
 * Implements hook_drush_command().
 */
function drushweb_client_drush_command() {
  $items['webservice'] = array(
    'description' => 'Execute a drush command on a site running Drush Webservice Server.',
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
  $authentication = NULL;
  $formatter = new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON);
  $client = new HttpClient($authentication, $formatter);

  global $base_url;
  $commands = drush_get_command();
  $commands = $commands['arguments'];
  // TODO support all options ?
  if (drush_get_context('DRUSH_AFFIRMATIVE')) {
    $commands[] = '--yes';
  }
  // TODO webservice / ?
  $request = new HttpClientRequest("$base_url/?q=" . DRUSHWEB_PATH, array(
    'method' => 'POST',
    'parameters' => NULL,
    'data' => implode(' ', $commands),
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
