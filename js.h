

String setRozklad(String time_vkl, String time_vukl, String time_zzz ) {
  //Встановлює значення розкладу в поля вводу форми HTML
  String s = String("<script> let input_vkl = document.getElementById(\"vkl\");")  +
             "let input_vukl  = document.getElementById(\"vukl\");" +
             "let input_zzz  =document.getElementById(\"zzz\");" +
             " if (\"" + time_vkl + "\"!= \"\") {input_vkl.value =  \"" + time_vkl + "\";}" +
             " if (\"" + time_vukl + "\" != \"\") {input_vukl.value  = \"" + time_vukl + "\"}" +
             " if (\"" + time_zzz + "\" != \"\") {input_zzz.value  =\"" + time_zzz + "\"}" +
             "</script>";
  return s;
}
String showTemp (byte temp) {
  // Оновлюємо значення температури на веб сторінці
  return "<script>let temp = document.querySelector(\".temp\");temp.innerHTML = \"Temperature " + String((int)temp) + "°С\";</script>";
}

String feedButton (bool anti_double_feed){
  String buttonClass ="";
  if (anti_double_feed){
    buttonClass = "btn-large disabled waves-effect waves-light";
    }
  else{
    buttonClass = "btn-large waves-effect waves-light";
  }
  return "<script>let feed = document.querySelector(\"#feedButton\"); feed.className=\""+ buttonClass +"\"; </script>";
  
}
