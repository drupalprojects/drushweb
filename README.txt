Description
===========
This module allows to run Drush without being granted access to the machine it relies on.
It may be useful on shared hosts that do not provide SSH access to servers.

It is made of 2 distinct projects:
 * 'Drush webservice Server' exposes Drush as a webservice secured with OAuth.
 * 'Drush webservice Client' extends Drush and allows to communicate with the client.

With the traditional way, one connects to the server using SSH and runs the following: 'drush enable field'.
With this module, one uses Drush directly on a workstation and runs the following: 'drush @mysite web enable field'.


Requirements
============
I - Server
----------
 * PHP not in safe-mode
 * PHP compiled with curl
 * Any version of Drush installed as a library is OK
 * Drush works best on Unix-like OS (Linux, OS X)

II - Client
-----------
 * PHP 5.3.3+
 * Drush 6.0.0+
 * Drush works best on Unix-like OS (Linux, OS X)


Installation for the impatient
==============================
1. On the server, enable drushweb_server.
2. On the server, install Drush as a library so this path is valid: site/all/libraries/drush/drush.info.
3. On the server, create a user and grant it perm 'Register OAuth consumers in Drush webservice' (please use a dedicated role).
4. On the server, go to /user/[nid]/oauth/consumer and add a consumer with a dummy callback. Note the key and the secret.
5. On the client, enter:
     drush web enable field
     --uri=<your-server-uri>
     --oauth_consumer_key=<your-oauth-key>
     --oauth_consumer_secret=<your-oauth-secret>
   The result of 'drush enable field' should show up.
6. Don't forget to check the sections 'Making the client user friendly' and 'Security considerations'.


Step-by-step installation
=========================
I - On the server: enable drushweb_server
-----------------------------------------
1. Copy the drushweb/ directory to sites/all/modules/.
2. Sign in a administrator.
3. Go to /admin/modules and enable the module 'Drush webservice Server'.

II - On the server: install Drush as a library
----------------------------------------------
1. Go to /admin/reports/status and note that 'Drush as a library' is not detected.
2. Download Drush from https://github.com/drush-ops/drush.
   You may select the branch/tag you wish and then click on the 'Download ZIP' button.
   e.g. Drush 4.5.0 can be downloaded at https://github.com/drush-ops/drush/archive/4.5.0.zip
3. Unzip the Drush archive in site/all/libraries/.
   The 'drush.info' file must be located at sites/all/libraries/drush/drush.info.
4. Go to /admin/reports/status and note that 'Drush as a library' is now detected.

III - On the server: generate oauth credentials
-----------------------------------------------
1. Go to /admin/people/create and create a user with these values:
   * Name => 'Drush webservice OAuth User' (suggestion)
   * Password => (whatever)
   * Email => (whatever)
   * Status => active 
   You may also use an existing user, but this is not recommended.
2. Grant the user the permission 'Register OAuth consumers in Drush webservice'.
   It is recommended to create a role dedicated to holding this permission.
3. Go to /user/[nid]/oauth/consumer (or edit the user and go to the 'OAuth consumers' tab).
4. Click on 'Add consumer'.
5. Use these values:
   * Consumer name => 'Drush webservice client' (suggestion)
   * Callback => 'http://example.net' (it won't be used)
   * Application context => 'Drush webservice'
6. Go to /user/[nid]/oauth/consumer (or edit the user and go to the 'OAuth consumers' tab).
7. Edit the consumer ('Drush webservice client').
8. Note the key and the secret.

IV - On the client: Test a command
----------------------------------
1. Install Drush as described on https://github.com/drush-ops/drush.
2. Copy the drushweb/ directory to ~/.drush/ so the client can be called from anywhere in the filesystem.
   You may also copy drushweb/ to /usr/share/drush/commands/ to make it available system-wide.
3. Clear Drush's caches: 'drush cache-clear drush'.
4. Have a look at the command help: 'drush help web'.
5. Test a command:
     drush web enable field
     --uri=<server-uri>
     --oauth_consumer_key=<oauth-key>
     --oauth_consumer_secret=<oauth-secret>
   The result of 'drush enable field' should show up.
6. Don't forget to check the section 'Making the client user friendly'.


Making the client user friendly
===============================
I - Global deployment
---------------------
The 'drush web' command becomes available using any of these contexts:
 * Available inside a Drupal instance with drushweb_client enabled.
 * Available anywhere in the filesystem by copying drushweb/ to ~/.drush/
 * Available system-wide by copying drushweb/ to /usr/share/drush/commands/
Copying drushweb/ to ~/.drush/ is probably the most convenient and secure way to use this module.

II - Interactivity
------------------
This module does not support interactivity, which means you can only send a command and see its result.
If a command prompts additional data or asks for a confirmation, 'no' will be inserted by default.
For this reason you will probably often use Drush's '--yes' option with this module.
Be sure to understand the implication of using the '--yes' option.

III - Aliases
-------------
The 3 required options (--uri, --oauth_consumer_key, --oauth_consumer_secret) can make this module a bit tedious.
A convenient way to get rid of this is to use Drush aliases.
An example of alias file can be found at http://drush.ws/examples/example.aliases.drushrc.php.

Example: if your '~/.drush/aliases.drushrc.php' file looks like this:
  <?php
  $aliases['mysite'] = array(
    'uri' => 'http://example.net/mysite', // Your server URI.
    'oauth_consumer_key' => 'y4ZYMrw8VNMDsnkibMxwu8ZybS4GPzHV', // Your OAuth key.
    'oauth_consumer_secret' => 'JHjnXtfpLxrPCVwnjyn8dCQmsxNmgfEZ', // Your OAuth secret.
  );
then this command: 'drush web enable field --uri=http://example.net/mysite --oauth_consumer_key=y4...HV --oauth_consumer_secret=JH...EZ'
can be simplified like this: 'drush @mysite web enable field'.


Security considerations
=======================
Keep in mind, despite the fact that it is secured with OAuth, that this module performs system() calls and exposes them to the Internet.
It is strongly recommended to observe the following:
 * Only run this module on HTTPS websites. Otherwise your OAuth credentials will be sent as clear text on the Internet.
 * Use a dedicated user to generate OAuth credentials. Hardening this user's permissions is a warranty that it can do nothing except executing Drush.
 * Use a dedicated role to hold the 'Register OAuth consumers in Drush webservices' permission. Only the dedicated user should be granted this permission.


Support
=======
Fengtan
http://drupal.org/user/847318
