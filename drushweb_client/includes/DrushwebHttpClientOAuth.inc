<?php

class DrushwebHttpClientOAuth extends DrushwebHttpClient {
  private $consumer_key;
  private $consumer_secret;

  public function __construct($cmd) {
    parent::__construct($cmd);
    // TODO make this dynamic (or drush prompt if not provided)
    $this->consumer_key = 'y4ZYMrw8VNMDsnkibMxwu8ZybS4GPzHV';
    $this->consumer_secret = 'JHjnXtfpLxrPCVwnjyn8dCQmsxNmgfEZ';
  }

  /**
   * @override
   */
  protected function getAuth() {
    $consumer = new DrupalOAuthConsumer($this->consumer_key, $this->consumer_secret);
    $sign = DrupalOAuthClient::signatureMethod();
    return new HttpClientOAuth($consumer, NULL, $sign, TRUE, TRUE);
  }

}
