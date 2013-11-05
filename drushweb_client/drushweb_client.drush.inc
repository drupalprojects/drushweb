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
    'description' => 'Execute the command on a site that runs Drush webservice Server.',
    'arguments' => array(
      'command' => 'A drush command to execute on the server.',
    ),
    'required-arguments' => TRUE,
    'options' => array(
      'uri' => array(
        'description' => 'Remote site URI.',
        'example_value' => 'http://example.com:80/mysite',
        'value' => 'required',
        'required' => TRUE,
      ),
      'oauth_consumer_key' => array(
        'description' => 'OAuth consumer key.',
        'example_value' => 'y4ZYMrw8VNMDsnkibMxwu8ZybS4GPzHV',
        'value' => 'required',
        'required' => TRUE,
      ),
      'oauth_consumer_secret' => array(
        'description' => 'OAuth consumer secret.',
        'example_value' => 'JHjnXtfpLxrPCVwnjyn8dCQmsxNmgfEZ',
        'value' => 'required',
        'required' => TRUE,
      ),
    ),
    'examples' => array(
      'drush webservice enable field' => 'Enable module field on a site.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('web'),
    //TODO drupal dependencies / drush dependencies
    //TODO allow-addition-options ? cf core-rsync
  );
  return $items;
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_drushweb_client_webservice() {
  // Format command to be sent.
  $cmd = drush_get_command();
  $cmd = $cmd['arguments'];
  if (drush_get_context('DRUSH_AFFIRMATIVE')) {// TODO support all options ? --verbose etc
    $cmd[] = '--yes';
  }
  $cmd = implode(' ', $cmd);

  // Get credentials.
  $uri    = drush_get_option('uri');
  $key    = drush_get_option('oauth_consumer_key');
  $secret = drush_get_option('oauth_consumer_secret');

  // Send and process request.
  try {
    $client = new DrushwebHttpClient($cmd, $key, $secret);
    $response = $client->execute($uri);
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
// TODO check drush minimum version
// TODO does it work when drushweb_client is disabled ?
