var xhr,imgName = "";
var histogram;
function getSourceFileName(){
  var w,h,wh,r;
  wh = getEl("size").value; w = parseInt(wh.split("x")[0]); h = parseInt(wh.split("x")[1]);
  r = "images/original/" + imgName + "/" + imgName;
  if(w > h) r += "_rect"; else r +="_square";
  r += ".jpg";
  return r;
}  
function getResizedName(){
  r = "images/" + imgName + "_" + getEl("size").value + ".jpg";
  return r;	
}

function uploadFile(fileInputName,callback){
  var file = document.getElementById(fileInputName).files[0];
  var formData = new FormData();
  formData.append('image',file,file.name);
  xhr = new XMLHttpRequest();
  xhr.open("POST","upload.php",true);
  xhr.onload = function(){
    if(xhr.status == 200) callback(xhr.responseText);
    else alert("Upload error");
  };
  xhr.send(formData);
}
function resizeImage(imgSource,imgResized,width,height,callback){
  var url = "resize.php?fileName=" + imgSource + "&imgDest=" + imgResized + "&width=" + width + "&height=" + height;
  getByUrl(url,function(imageUrl){
    callback(imageUrl);
  });
}
function getEl(id){
  return document.getElementById(id);
}
function recolor(imgId,targetId,red,green,blue){
  var ctxData = getImageData(imgId);
  var data = ctxData.data;
  var i,r,g,b;
  for(i = 0; i < data.length; i+=4){
    data[i] = (data[i] > red)? 255:0;
	data[i + 1] = (data[i + 1] > green)? 255:0;
	data[i + 2] = (data[i + 2] > blue)? 255:0;
  }
  var recolored = document.getElementById(targetId);
  var recoloredCtx = recolored.getContext('2d');
  recolored.height = ctxData.height;
  recolored.width = ctxData.width;  
  recoloredCtx.putImageData(ctxData, 0, 0);
}
function getImageData(imgId){
  var img = getEl(imgId);
  var c = document.createElement('canvas');
  c.width = img.width;
  c.height = img.height;
  var ctx = c.getContext('2d');
  ctx.drawImage(img, 0, 0);
  return ctx.getImageData(0, 0, img.width, img.height);
}
function createUpload(canvasId,uploadId){
  var w,h,ctxData,data,zigzag;
  ctxData = getImageData(canvasId);
  data = ctxData.data;
  w = ctxData.width;
  h = ctxData.height / 2;
  createGraphicsBuffer();
  getEl(uploadId).value = createFileJS();
  createHistogram(canvasId,"histogram");
  function createGraphicsBuffer(){
	var i,y,x;
    zigzag = new Uint8Array(w * h);
    i = 0;
    for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
	    zigzag[i] = getValue(data,( y * w + x) * 4) + (getValue(data,((y + h) * w + x) * 4) << 4);
	    i++;
	  } 
    }	
  }
  function createFileJS(){
	var i,j,js,mode;
	mode = getEl("fileMode").value;
	switch (mode){
	  case "Storage":
	    js = "var st = require('Storage');\n";break;
      case "fs":
	    js = "var fs = require('fs');\n"; break;
	}
    js += "var fileName = '" + imgName + ".bin';\n";
    for(i = 0; i < h; i++){
	  switch (mode){
		case "Storage":
	      js += "st.write(fileName,["; break;
		case "fs":
		  if(i == 0)
			js += "fs.writeFile(fileName,new Uint8Array([";
		  else
		    js += "fs.appendFile(fileName,new Uint8Array(["; 
		  break;
	  }
	  for(j = 0; j < w; j++){
	    if(j > 0) js += ",";
        js += zigzag[i * w + j];
	  }
	  switch (mode){
		case "Storage":
		  js += "]," + i * w + ");\n"; 
          if(i == 0) js += "," + w * h;
          break;
        case "fs":
		  js += "]));\n";
		  js += "for(i = 0; i < 10000;i++){x = i;}\n";
          break;		
	  }  
    }
	switch (mode){
      case "Storage": break;
	  case "fs": js += "console.log('Hurra ',fileName,fs.statSync(fileName));\n\n";
	}
	return js;
  }
}
function createHistogram(SourceId,histogramId){
  var ctxData,histogram,i;
  ctxData = getImageData(SourceId).data;
  histogram = [0,0,0,0,0,0,0,0];
  for(i = 0; i < ctxData.length; i+=4) histogram[getValue(ctxData,i)]++;
  drawBars(histogram,histogramId);
}
function drawBars(data,id){
  var i,x,y,w,h,mx,cols;
  var c = getEl(id);
  var ctx = c.getContext('2d');
  cols = ["#000","#00f","#0f0","#0ff","#f00","#f0f","#ff0","#fff"];
  mx = Math.max(...data);
  ctx.clearRect(0,0,50,50);
  for(i = 0; i < data.length; i++){
	x = i * 50 / data.length; y = 50;
	w = 50 / (data.length + 1); h = -50 / mx * data[i];
	rect(x,y,w,h,cols[i]);
  } 
  function rect(x,y,w,h,col){
console.log(x,y,w,h);
    ctx.fillStyle = col;
    ctx.fillRect(x,y,w,h);
    ctx.strokeStyle = "#000";
    ctx.beginPath();
    ctx.rect(x,y,w,h);
    ctx.stroke(); 	  
  }
}
function getValue(data,i){
  var v=0;
  if(data[i] > 0) v += 4;
  if(data[i + 1] > 0) v += 2;
  if(data[i + 1] > 0) v += 1;
  return v;
}
function getImagesOrigin(callback){
  getByUrl("getOriginal.php",function(response){
    var images = JSON.parse(response);
    for(var i = 0; i < images.length; i++){
      images[i] = images[i].split("/")[2];
	}
    callback(images);
  });
}
function getByUrl(url,callback){
  xhr = new XMLHttpRequest();
  xhr.open("GET",url,true);
  xhr.onload = function(){
    if(xhr.status == 200) callback(xhr.responseText);
	else alert("error in:" + url);
  };
  xhr.send();
}
function fillSelect(id,data){
  var el = getEl(id);
  el.options.length = 0;
  for(var i = 0; i < data.length; i++){
	if(data[i])el.options.add(new Option(data[i]));
  }
}