<?PHP

if(empty($_ARGS[1]))
	die('Bad call to /forgot -- needs a token as first parameter.');

$token = fetch_pw_reset($_ARGS[1]);

if(empty($token))
	die('The password reset token "'.$_ARGS[1].'" no longer exists. This means someone (maybe you) has already used it to reset a password.');

$login = $token['login'];
$token_timestamp = strtotime($token['generated']);
$token_age = time() - $token_timestamp;

$token_date = date('Y-m-d H:i:s', $token_timestamp);

$too_old = false;
if($token_age > 3600) # 3600 seconds == one hour
	$too_old = true;
?>
<h1>Resetting password for <?PHP echo $login; ?>:</h1>

<?PHP
if(!empty($_POST['newpass']) and !$too_old)
{
	if($_POST['newpass'] != $_POST['pass_confirm'])
	{
		echo '<p style="color: #f00">The password doesn\'t match the confirmation. Retype them?</p>';
		goto stop_password_processing;
	}

	$salt = fmt_strgen(16);
	$rounds = mt_rand(64,128);

	$hash = hash_password($token['login'], $_POST['newpass'], $salt, $rounds);

	if(!store_pw_hash($token['user_id'], $hash, $salt, $rounds))
	{
		echo '<p style="color: #f00">Something went wrong when storing the new password. Please complain to an administrator.</p>';
		goto stop_password_processing;
	}

	delete_pw_reset($_ARGS[1]);

	# TODO: Link "log in" to the login page
	echo '<p>Success! You should now be able to log in with your new password.</p>';

	die();
}

stop_password_processing: # Yep, it's a label. Seriously.

?>
<p>Your password reset token was generated at <?PHP echo $token_date; ?> UTC
(<?PHP echo fmt_duration($token_age); ?>).</p>
<?PHP
if($too_old)
{
	echo '<p>This is too long for security purposes (if anyone had stolen your '
		.'token, they could\'ve changed your password for you!), so you\'ll '
		.'have to click the "forgot password" link again.</p>';
		# TODO: Link "forgot password" when we have a page for that.
}
?>
<?PHP
if($too_old)
	exit();
?>
<form method="post" style="text-align: right; width: 300px">
	New password: <input type="password" name="newpass" /><br/>
	Confirm: <input type="password" name="pass_confirm" /><br/>
	<input type="submit" />
</form>
