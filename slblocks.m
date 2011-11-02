function blkStruct = slblocks
  % Specify that the product should appear in the library browser
  % and be cached in its repository
  Browser.Library = 'ecan_dspic';
blkStruct.OpenFcn = 'ecan_dspic';
  Browser.Name    = 'dsPIC ECAN Library';
  blkStruct.Browser = Browser;
