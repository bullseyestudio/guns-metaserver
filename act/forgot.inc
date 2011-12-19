<?PHP

if(empty($_ARGS[1]))
	die('Bad call to /forgot -- needs a token as first parameter.');

$sth = $dbh->prepare('SELECT `u`.`login`, `t`.`generated` FROM `pw_tokens` AS `t`
	LEFT JOIN `users` AS `u`
		ON `u`.`id` = `t`.`user_id`
	WHERE `token` = ?');
$sth->execute(array($_ARGS[1]));

$row = $sth->fetch(PDO::FETCH_ASSOC);
$sth->closecursor();

$login = $row['login'];
$token_timestamp = strtotime($row['generated']);
$token_age = time() - $token_timestamp;

$token_date = date('Y-m-d H:i:s', $token_timestamp);

$too_old = false;
//if($token_age > 3600) # 3600 seconds == one hour
//	$too_old = true;
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

	# TODO: Continue this tomorrow.

	print_r($_POST);
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
