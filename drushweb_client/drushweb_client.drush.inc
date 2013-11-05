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
    'allow-additional-options' => TRUE,
    'examples' => array(
      'drush webservice enable field' => 'Enable module field on a site.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('web'),
    'drupal dependencies' => array(
      'http_client',
      'http_client_oauth',
    ),
  );
  return $items;
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_drushweb_client_webservice() {
  // Format command to be sent.
  $cmd = implode(' ', drush_get_context('DRUSH_COMMAND_ARGS'));

  // Get credentials.
  $uri    = drush_get_option('uri');
  $key    = drush_get_option('oauth_consumer_key');
  $secret = drush_get_option('oauth_consumer_secret');

  // Send and process request.
  try {
    $client = new DrushwebHttpClient($cmd, $key, $secret);
    drush_log(dt('Sent command \'@cmd\' to @uri', array('@cmd' => $cmd, '@uri' => $uri)));
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

class DrushwebHttpClient {
  private $cmd;
  private $client;

  public function __construct($cmd, $oauth_key, $oauth_secret) {
    $this->cmd = $cmd;
    $consumer = new DrupalOAuthConsumer($oauth_key, $oauth_secret);
    $sign = DrupalOAuthClient::signatureMethod();
    $auth = new HttpClientOAuth($consumer, NULL, $sign, TRUE, TRUE);
    $formatter = new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON);
    $this->client = new HttpClient($auth, $formatter);
  }

  /**
   * Send and process HTTP request.
   */
  public function execute($url) {
    try {
      return $this->client->post($url, $this->cmd, array('q' => 'drushweb/drush/execute'));
    }
    catch (HttpClientException $e) {
      switch ($e->getCode()) {
        case 301:
        case 302:
        case 303:
        case 305:
        case 307:
          $headers = $this->parseHeaders($e->getResponse()->headers);
          return $this->execute($headers['location']);
        default:
          throw $e;
      }
    }
  }

  /**
   * Parse HTTP headers in associative array
   *
   * @todo http_parse_headers (pecl extension).
   * @todo drupalGetHeaders (cf simpletest).
   */
  protected function parseHeaders($content) {
    $headers = array();
    $lines = explode("\n", $content);
    foreach ($lines as $line) {
      list($name, $value) = explode(':', $line, 2);
      if ($value) {
        $headers[strtolower($name)] = trim($value);
      }
    }
    return $headers;
  }

}
