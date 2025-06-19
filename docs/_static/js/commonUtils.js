$(function ($) {
  // 回到顶部
  $('.to-top').click(function () {
    $('.wy-nav-content').scrollTop(0)
  })
  // 图片放大
  document.querySelectorAll('img:not([data-no-viewer])').forEach(img=>{new Viewer(img,{toolbar: false})})
});