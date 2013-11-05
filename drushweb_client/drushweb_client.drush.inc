<?php
/**
 * @file
 * Drush webservice Client module drush integration.
 */

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

  global $base_url;
  $client = new DrushwebHttpClient($cmd);
  try {
    // Drupal's base url is a directory, so we append a slash to avoid a useless301 redirection.
    $response = $client->execute($base_url . '/');
    _drushweb_client_print_response($response);
  }
  catch (HttpClientException $e) {
    $msg = dt('Failed to communicate with the server: !msg', array('!msg' => $e->getMessage()));
    return drush_user_abort($msg);
  }
}

/**
 * Format and print the body of the server response.
 *
 * @param $body
 *   Http response body.
 */
function _drushweb_client_print_response($body) {
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
