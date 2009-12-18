basename='t2top1_ps10_';
for u=1:3
    mfile = sprintf('%s%d',basename,u);
    eval(mfile);
    data{u}=ncdata;
end
