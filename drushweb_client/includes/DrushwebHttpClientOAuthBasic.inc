<?php

class DrushwebHttpClientOAuthBasic extends DrushwebHttpClientOAuth {
  private $http_user;
  private $http_pass;

  public function __construct($cmd) {
    parent::__construct($cmd);
    // TODO make this dynamic (drush_prompt)
    $this->http_user = 'admin';
    $this->http_pass = 'abcd1234';
  }

  /**
   * @override
   */
  protected function getAuth() {
    $consumer = new DrupalOAuthConsumer($this->consumer_key, $this->consumer_secret);
    $sign = DrupalOAuthClient::signatureMethod();
    return new HttpClientOAuthBasic($this->http_user, $this->http_pass, $consumer, NULL, $sign, TRUE, TRUE);
  }

}
