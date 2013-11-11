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
    drushweb_client_print_response($client->execute($uri));
  }
  catch (Exception $e) {
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
function drushweb_client_print_response($body) {
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


class DrushwebHttpClient {
  private $cmd;
  private $oauth_key;
  private $oauth_secret;
  private $oauth_token = '';
  private $oauth_version = '1.0';
  private $http_method = 'POST';
  private $server_path = 'drushweb/drush/execute';

  public function __construct($cmd, $oauth_key, $oauth_secret) {
    $this->cmd = $cmd;
    $this->oauth_key = $oauth_key;
    $this->oauth_secret = $oauth_secret;
  }

  /**
   * Send and interpret HTTP request.
   *
   * @param $url
   *   URL of server running Drush webservice Server.
   */
  public function execute($url) {
    // Normalize url and serialize data.
    $url = $this->normalize_http_url($url);
    $data = drupal_json_encode($this->cmd);

    // Build query parameters.
    $parameters = array(
      'q' => $this->server_path,
      'oauth_version' => $this->oauth_version,
      'oauth_nonce' => md5(microtime() . mt_rand()),
      'oauth_timestamp' => time(),
      'oauth_consumer_key' => $this->oauth_key,
      'oauth_body_hash' => base64_encode(sha1($data, TRUE)),
      'oauth_signature_method' => 'HMAC-SHA1',
    );
    $parameters['oauth_signature'] = $this->build_signature($url, $parameters);
    
    // Build HTTP headers.
    $headers = array(
      'Content-Type' => 'application/json',
      'Content-Length' => strlen($data),
      'Accept' => 'application/json',
    );
    
    // Send query and interpret response.
    $response = $this->curl($url, $parameters, $headers, $data);
    switch ($response->responseCode) {
      case 200:
      case 201:
      case 202:
      case 203:
      case 204:
      case 205:
      case 206:
        return drupal_json_decode($response->body);
      case 301:
      case 302:
      case 303:
      case 305:
      case 307:
        return $this->execute($response->headers['location']);
      default:
        throw new Exception(check_plain($response->responseMessage));
    }
  }

  /**
   * Execute an HTTP request using curl.
   *
   * @param $url
   *   URL of server.
   * @param $parameters
   *   Array of HTTP query parameters.
   * @param $headers
   *   Array of HTTP headers.
   * @param $data
   *   Data to send in request.
   * @return object
   *  The interpreted response.
   */
  public function curl($url, $parameters, $headers, $data) {
    // Format headers.
    foreach ($headers as $key => $val) {
      $headers[$key] = "$key: $val";
    }

    // Prepare curl options.
    $chr = curl_init();
    curl_setopt($chr, CURLOPT_USERAGENT, 'Drupal (+http://drupal.org/)');
    curl_setopt($chr, CURLOPT_HEADER, 1);
    curl_setopt($chr, CURLOPT_CUSTOMREQUEST, $this->http_method);
    curl_setopt($chr, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($chr, CURLOPT_URL, $url . '?' . $this->build_http_query($parameters));
    curl_setopt($chr, CURLOPT_POSTFIELDS, $data);
    curl_setopt($chr, CURLOPT_HTTPHEADER, $headers);
    curl_setopt_array($chr, array());
    
    // Send request and interpret response.
    $response = curl_exec($chr);
    $error = curl_error($chr);
    curl_close($chr);
    if ($error) {
      throw new Exception('Curl Error: ' . $error);
    }
    return $this->interpret_http_response($response);
  }

  /**
   * Encode a string or an array using RFC3986.
   *
   * @param $input
   *   String or array to encode.
   * @return
   *   Encoded string or array.
   */
  public function urlencode_rfc3986($input) {
    if (is_array($input)) {
      return array_map(array($this, 'urlencode_rfc3986'), $input);
    }
    if (is_scalar($input)) {
      $encoded = rawurlencode($input);
      $encoded = str_replace('%7E', '~', $encoded);
      $encoded = str_replace('+',   ' ', $encoded);
      return $encoded;
    }
    return '';
  }

  /**
   * Interpret a raw HTTP response.
   *
   * @param $response
   *   String to interpret.
   * @return
   *   Object with interpreted response.
   */
  public function interpret_http_response($response) {
    // Split response.
    $split = preg_split('/\r\n\r\n/', $response, 2);
    if (!isset($split[1])) {
      throw new Exception('Error interpreting response');
    }

    // Retrieve raw headers and body.
    list($headers_raw, $body) = $split;
    $obj = new StdClass;
    $obj->headers_raw = $headers_raw;
    $obj->body = $body;
    
    // Format headers in an array.
    $obj->headers = array();
    $lines = explode("\n", $headers_raw);
    foreach ($lines as $line) {
      list($name, $value) = explode(':', $line, 2);
      if ($value) {
        $obj->headers[strtolower($name)] = trim($value);
      }
    }
    
    // Retrieve HTTP response code and message.
    $matches = array();
    if (preg_match('/HTTP\/1.\d (\d{3}) (.*)/', $headers_raw, $matches)) {
      $obj->responseCode = intVal(trim($matches[1]), 10);
      $obj->responseMessage = trim($matches[2]);
      // Handle HTTP/1.1 100 Continue.
      if ($obj->responseCode == 100) {
        return $this->interpret_http_response($body);
      }
    }
    return $obj;
  }

  /**
   * Normalize a URL in the form 'scheme://host/path'.
   *
   * @param $url
   *   URL to normalize.
   * @return
   *   Normalized URL.
   */
  public function normalize_http_url($url) {
    $parts = parse_url($url);
    $scheme = (isset($parts['scheme'])) ? $parts['scheme'] : 'http';
    $port = (isset($parts['port'])) ? $parts['port'] : (($scheme == 'https') ? '443' : '80');
    $host = (isset($parts['host'])) ? strtolower($parts['host']) : '';
    $path = (isset($parts['path'])) ? $parts['path'] : '';
    if (($scheme == 'https' && $port != '443') || ($scheme == 'http' && $port != '80')) {
      $host = "$host:$port";
    }
    return "$scheme://$host$path";
  }

  /**
   * Build signature.
   *
   * @param $url
   *   URL of the server.
   * @param $parameters
   *   Array of HTTP parameters.
   * @return
   *   Signature, as a string.
   */
  public function build_signature($url, $parameters) {
    $string = array($this->http_method, $url, $this->build_http_query($parameters));
    $string = $this->urlencode_rfc3986($string);
    $string = implode('&', $string);
    $key = $this->urlencode_rfc3986(array($this->oauth_secret, $this->oauth_token));
    $key = implode('&', $key);
    return base64_encode(hash_hmac('sha1', $string, $key, TRUE));
  }

  /**
   * Build an HTTP query from parameter.
   *
   * @param $params
   *   Array of parameters, keyed by parameter key.
   * @return
   *   String in the form 'key1=val1&key2=val2'.
   */
  public function build_http_query($params) {
    // Urlencode both keys and values
    $keys = $this->urlencode_rfc3986(array_keys($params));
    $values = $this->urlencode_rfc3986(array_values($params));
    $params = array_combine($keys, $values);

    // Sort parameters by name and then by value.
    uksort($params, 'strcmp');
    $pairs = array();
    foreach ($params as $parameter => $value) {
      if (is_array($value)) {
        sort($value, SORT_STRING);
        foreach ($value as $duplicate_value) {
          $pairs[] = $parameter . '=' . $duplicate_value;
        }
      } else {
        $pairs[] = $parameter . '=' . $value;
      }
    }

    // Glue parameters with a '&'.
    return implode('&', $pairs);
  }

}
