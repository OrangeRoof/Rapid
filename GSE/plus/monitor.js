// the idea is to have as little js as possible. So jinja will be responsible for generating chart templates,
// all this does it 1) get data to populate the charts and 2) updating the data as it comes in

function initialize(){
  $( "div[class=container]" ).each(function(i, e){
    var div = document.createElement("div")
    div.classList.add('chart-container')
    var canvas = document.createElement("canvas")
    div.appendChild(canvas);
		e.appendChild(div);
    var ctx = canvas.getContext('2d');
    new Chart(ctx, {
      type: 'line',
      options: {
        responsive: true,
        animation: {
          easing: 'linear'
        },
      }
    });
  })
}

window.onload = () => {
  initialize();
}

function get_and_parse(port){
  $.ajax({
  url: "../get?" + "port",
  data: 'json',
  cache: false
})
  .done(function(data) {
    print(data)
  });
}