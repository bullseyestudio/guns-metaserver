<?PHP
# Formatting functions.

function fmt_duration($secs, $short = false, $drop_secs = false)
{
	if(empty($secs) or ($drop_secs and $secs < 60))
		return 'just now';

	$suffix = ' ago';
	if($secs < 0)
	{
		$suffix = ' from now';
		$secs = abs($secs);
	}

	$mins = floor($secs / 60);
	$hrs = floor($mins / 60);

	$secs = $secs % 60;
	$mins = $mins % 60;

	$str = '';

	if(!empty($hrs))
	{
		$str .= $hrs.($short ? 'h' : ' hour').($hrs != 1 ? 's' : '');
	}

	if(!empty($mins))
	{
		if(!empty($hrs))
			$str .= ($short ? ' ' : ', ');

		$str .= $mins.($short ? 'm' : ' minute').($mins != 1 ? 's' : '');
	}

	if(!empty($secs) and !$drop_secs)
	{
		if(!empty($hrs) or !empty($mins))
			$str .= ($short ? ' ' : ', ');

		$str .= $secs.($short ? 's' : ' second').($secs != 1 ? 's' : '');
	}

	return $str.$suffix;
}

function fmt_strgen($len = 8, $valid_regex = '/[a-z0-9]/i')
{
	$str = '';
	$rejects = 0;
	$gen_len = 0;

	while($gen_len < $len)
	{
		$ch = chr(mt_rand(32, 127));

		if(!preg_match($valid_regex, $ch))
		{
			if($rejects++ > ($len * 8))
				die('Too many rejects trying to generate string of length '.$len.' with regex '.$valid_regex);

			continue;
		}

		$str .= $ch;
		$gen_len++;
	}

	return $str;
}

?>
