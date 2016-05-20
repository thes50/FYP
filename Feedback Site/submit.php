<?php
	$db = null;
	
	try 
	{
		$db = new PDO('mysql:host=mysql.hostinger.co.uk;dbname=u284288850_db','u284288850_user','LXjFCYk8eI');
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	}
	catch(PDOException $e){
	}

	if($_POST["Q1"] == null || 
	   $_POST["Q2"] == null ||
	   $_POST["Q3"] == null ||
	   $_POST["Q4"] == null)
	{
		//Answers invalid.
		header('Location:error.php');
	}

	$stmt = $db->prepare("INSERT INTO results
		(Q1,Q2,Q3,Q4,Q5,code)
		VALUES (:q1, :q2, :q3, :q4, :q5, :code)");
	$stmt->bindParam(':q1', $_POST["Q1"]);
	$stmt->bindParam(':q2', $_POST["Q2"]);
	$stmt->bindParam(':q3', $_POST["Q3"]);
	$stmt->bindParam(':q4', $_POST["Q4"]);
	$stmt->bindParam(':q5', $_POST["Q5"]);
    $code = getCode();

	$stmt->bindParam(':code', $code);
	$stmt->execute();


	function getCode()
	{
		$options = array(
			a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,
			A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
			1,2,3,4,5,6,7,8,9
		);

		$code = "";
		for($i = 0; $i < 10; $i++)
		{
			$code .= $options[rand(0,60)];
		}
		return $code;
	}
?>	

<html>
    <head>
		<link href='https://fonts.googleapis.com/css?family=Lato' rel='stylesheet' type='text/css'>
		<link href="style/page.css" rel='stylesheet' type='text/css' />
		<title>Thanks!</title>
    </head>
    <body>
         <div class="main">
         <p>Thank you for your feedback, while no personal data has been gathered you can request to have your answers removed by quoting the passphrase: <b><?php echo $code; ?></b> in an email to: <i>surveyremoval@outlook.com</i>
         </div>
    </body>
</html>						