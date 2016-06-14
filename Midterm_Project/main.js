window.onload = function()
{
	x = 0;
	y = 0;
	speed = 5;
	angle = 0;
	mod = 0;
	var MAX_Height =20;
	x_old=0;
	y_old=0;
	
	canvas = document.getElementById("canvas");
	context = canvas.getContext("2d");
	car = new Image();
		car.src="starcraft_terran_battlecruiser_by_voidwar-d7ltbe2.png"
	car.style.width = '50%'
	//car.style.height = 'auto'
	
    window.addEventListener("keydown", keypress_handler, false);
    window.addEventListener("keyup", keyup_handler, false);

	var move = setInterval(function()
	{
		draw();
	}, 30);
};

function draw()
{
	var boundh = 600;
	var boundw = 1200;
	context = canvas.getContext("2d");
	context.clearRect(0, 0, boundw, boundh);

	context.fillStyle = "rgb(0, 0, 220)";
	context.fillRect(50, 50, 100, 100);

	x_old =x;
	y_old =y;

	x += (speed*mod) * Math.cos(Math.PI/180 * angle);
	y += (speed*mod) * Math.sin(Math.PI/180 * angle);
	//
	context.moveTo(x_old,y_old);
	context.lineTo(x,y);
	context.lineWidth = 5;
      	context.strokeStyle = '#3271e7';
      	context.lineCap = 'round';
	context.stroke();
	//

	context.save();
	context.translate(x, y);
	context.rotate(Math.PI/180 * angle);
	var ratio = 0.2;
	context.drawImage(car, -(car.width*ratio/2), -(car.height*ratio/2),ratio*car.width,ratio*car.height);	
//	context.drawImage(car, 0, 0,0.5*car.width,0.5*car.height);	
	context.restore();
	//x_old = x;
	//y_old = y;

}

function keyup_handler(event)
{
	if(event.keyCode == 87 || event.keyCode == 83)
	{
		mod = 0;
	}
}

function keypress_handler(event)
{
	console.log(event.keyCode);
	if(event.keyCode == 87)
	{
		mod = 1;
	}
	if(event.keyCode == 83)
	{
		mod = -1;
	}
	if(event.keyCode == 65)
	{
		angle -= 5;
	}
	if(event.keyCode == 68)
	{
		angle+=5;
	}
}

function render(src){
	var MAX_HEIGHT =10;
	var image = new Image();
	image.onload = function(){
		var canvas = document.getElementById("myCanvas");
		if(image.height > MAX_HEIGHT) {
			image.width *= MAX_HEIGHT / image.height;
			image.height = MAX_HEIGHT;
		}
		var ctx = canvas.getContext("2d");
		ctx.clearRect(0, 0, canvas.width, canvas.height);
		canvas.width = image.width;
		canvas.height = image.height;
		ctx.drawImage(image, 0, 0, image.width, image.height);
	};
	image.src = src;
	return image;

}
