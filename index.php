<?PHP
$_ARGS = array();
if(!empty($_GET['args']))
{
	$_ARGS = explode('/', $_GET['args']);
}

include('inc/config.inc');
include('inc/sql.inc');

define('PATH_BASE',  dirname(__FILE__));
define('PATH_ACT',   PATH_BASE.'/act');

$action = 'home';
if(!empty($_ARGS[0]))
	$action = $_ARGS[0];

$act_file = PATH_ACT.'/'.$action.'.inc';
if(!is_file($act_file))
	die('The action "'.$action.'" cannot be executed. Check your URL.');

include($act_file);
?>
