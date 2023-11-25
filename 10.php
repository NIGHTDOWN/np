<?php  
  //多边形对象
  class Polygon
  {
      public $points;
      public $area;
      private $vertices = [];
      public function __construct($points)
      {
          $this->points = $points;
          $this->area = calculateArea($this->points);
      }
      public function draw($image)
      {
          // $image = imagecreatetruecolor(1000, 1000);
          // $text_color = $color ?? imagecolorallocate($image, 255, 255, 255);
          $text_color=rand(0,999999);
          imagefilledpolygon($image, $this->vertices(),(count($this->points))/2, $text_color);
      }
      public function vertices()
      {
         
          foreach ($this->points as $key => $value) {
              # code...
              array_push($this->vertices, $value[0], $value[1]);
          }
          return $this->vertices;
      }
      
  }
  //图片对象
  class image
  {
      private $image;
      public function __construct($x, $y)
      {
          $this->image = imagecreatetruecolor($x, $y);
      }
      public function save()
      {
          $str = "pic/" . rand(111, 999999) . ".png";
          imagepng($this->image, $str);
          // 释放内存  
          imagedestroy($this->image);
          return $str;
      }
      public function getres()
      {
          return $this->image;
      }
  }
  //遗传挤压算法
  class calc{
    // 初始化种群大小
    private $populationSize = 100;
    // 生成初始种群
    private $population = array();
  }