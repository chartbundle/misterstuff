$fn=32;
x=158.75+6.35+7.52;
xminus=6.35;
xca=6.35+7.52;
wca=158.75;
y=154.94+10.16+(0.3*25.4);
yminus=10.16;

holes=[[0,0],[-157.48,22.86],[0,154.94],[-157.48,154.94]];
hro=4;
hri=1.9;
baseh=2;
hh=baseh+0.01;

e=0.1;
e2=e*2;

// Enable by uncommenting

//pixy=[-145.10,0,0];
//rockxy=[-75,75,0];
dexy=[-75,25,0];
//picoxy=[18,25,0];
//hubxy=[90,0,0];
//powerdistxy=[40,0,0];
//ecpcbxy=[0,0];
//powerxy=[60,65,0];
//kvmxy=[60,90,0];
//relayxy1=[90,90,0];
//relayxy2=[90,90,0];
//switchxy=[-70,-50,0];

// holes=0 no holes
// holes=1 only holes

// Holes for sides
mountri=3.6/2;
mountro = 6/2;
// Holes for device mounting, using heatset inserts
// Not all devices use these
mountr0=4.6/2;
mountr1=4.1/2;

// Height of the side holes
mounth= 3;

mountoff=8;
mountw=3.25;

module holeat(x,y,r1,r2,h,e=0,cs=0) {
        if(cs != 0 ) {
           translate([x,y,-e]) 
             cylinder(r1=r1,r2=r2,h=cs+(2*e));
           translate([x,y,-e]) 
             cylinder(r1=r2,r2=r2,h=h+(2*e));
        } else {
           translate([x,y,-e]) 
             cylinder(r1=r1,r2=r2,h=h+(2*e));
                }
     };

module mixmesh(a,b,r,h,e=0,cs=0) {
    for (i=a){
        for (j=b){
        if (i != j ){
            hull() {
           holeat(i[0],i[1],r,r,h,e);
           holeat(j[0],j[1],r,r,h,e);
            };
             };
       };
      };
};

module doit(holes,outer,hole,cr,ourbaseh,hor,hir1,hir2,hh,csh=0) {
if (holes != 1 ) {
        mixmesh(outer,outer,cr,ourbaseh);
    for (i=hole){
           if (is_num(hor)) {
           holeat(i[0],i[1],hor,hor,hh);
           } else {
           holeat(i[0],i[1],hor[0],hor[1],hh);
           }
      };
};     
if (holes != 0 ) {
    for (i=hole){
        holeat(i[0],i[1],hir1,hir2,hh,e,cs=csh);
      };

};

};

module pi(holes=0) {
pix=56;
piy=85;
picornerr=3;
pibaseh=baseh;
piouter = [[0+picornerr,0+picornerr],[pix-picornerr,0+picornerr],
    [0+picornerr,piy-picornerr],[pix-picornerr,piy-picornerr]];
pihole=[[3.5,3.5],[3.5,58+3.5],
        [3.5+49,3.5],[3.5+49,3.5+58]];
piholero = [5,3];
// piholeri1 = 2.7/2;
// piholeri2 = 2.7/2;
//piholeri1 = 3.8/2;
//piholeri2 = 3.4/2;
piholeri1=mountr0;
piholeri2=mountr1;
// 3.4/2,3.8/2
piholeh=6;


pimount = [[3.5-mountoff,piy/2],[3.5+49+mountoff,piy/2]];

doit(holes,piouter,pihole,picornerr,pibaseh,piholero,piholeri1,piholeri2,piholeh);
doit(holes,concat(piouter,pimount),pimount,mountw,pibaseh,mountro, mountri,mountri,mounth);

};

module rock(holes=0) {
rockx=74.25;
rocky=100.20;
rockcornerr=3;
rockbaseh=baseh;
rockouter = [[0+rockcornerr,0+rockcornerr],[rockx-rockcornerr,0+rockcornerr],
    [0+rockcornerr,rocky-rockcornerr],[rockx-rockcornerr,rocky-rockcornerr]];
rockhole=[[21.85,3.525],[21.85,96.675],
        [21.85+49.05,3.525],[21.85+49.05,96.675]];
rockholero = 3;
// rockholeri1 = 2.7/2;
// rockholeri2 = 2.7/2;
rockholeri1 = mountr0;
rockholeri2 = 3.3/2;
// 3.4/2,3.8/2
// NVMe 13mm  original 6
rockholeh=16;
rmountoff=mountoff+3;

rockmount = [[rockcornerr-rmountoff,rocky/2],[rockx+rmountoff-rockcornerr,rocky/2]];

doit(holes,rockouter,rockhole,rockcornerr,rockbaseh,rockholero,rockholeri1,rockholeri2,rockholeh);
doit(holes,concat(rockouter,rockmount),rockmount,mountw,rockbaseh,mountro, mountri,mountri,mounth);
     
};


module de10n(holes=0) {
dex=65.41+3.18;
dey=103.62+3.18;
decornerr=3.18;
debaseh=baseh;
deouter = [[0+decornerr,0+decornerr],[dex-decornerr,0+decornerr],
    [0+decornerr,dey-decornerr],[dex-decornerr,dey-decornerr]];
dehole=[[decornerr,decornerr],[decornerr,dey-decornerr],
        [dex-decornerr,decornerr],[dex-decornerr,dey-decornerr]];

//deholero = 6.35/2;
deholero = [5,3];

deholeri1 = mountr0;
deholeri2 = mountr1;
deholeh=6;
tabx=10;
taby=20;
//if (holes == 0 ) {
//    translate([dehole[3][0],decornerr+dehole[3][1]-taby,0]) cube([tabx,taby,debaseh]);
//    };

demount = [[-mountoff,dey/2],[dex+mountoff,dey/2]];

doit(holes,deouter,dehole,decornerr,debaseh,deholero,deholeri1,deholeri2,deholeh);
doit(holes,concat(deouter,demount),demount,mountw,debaseh,mountro,mountri,mountri,mounth);
        
      
};

module pico(holes=0) {
dex=51;
dey=85;
decornerr=0;
debaseh=baseh;
deouter = [[0+decornerr,0+decornerr],[dex-decornerr,0+decornerr],
    [0+decornerr,dey-decornerr],[dex-decornerr,dey-decornerr]];
dehole=[[decornerr,decornerr],[decornerr,dey-decornerr],
        [dex-decornerr,decornerr],[dex-decornerr,dey-decornerr]];

//deholero = 6.35/2;
deholero = 3;
deholeri1 = 3.8/2;
deholeri2 = 3.4/2;
deholeh=6;
tabx=10;
taby=20;
//if (holes == 0 ) {
//    translate([dehole[3][0],decornerr+dehole[3][1]-taby,0]) cube([tabx,taby,debaseh]);
//    };

demount = [[-mountoff,dey/2],[dex+mountoff,dey/2]];

doit(holes,deouter,dehole,decornerr,debaseh,deholero,deholeri1,deholeri2,deholeh);
doit(holes,concat(deouter,demount),demount,mountw,debaseh,mountro,mountri,mountri,mounth);
        
      
};

module power(holes=0) {
dex=97;
dey=99;
decornerr=4.5;

debaseh=baseh;
deouter = [[0+decornerr,0+decornerr],[dex-decornerr,0+decornerr],
    [0+decornerr,dey-decornerr],[dex-decornerr,dey-decornerr]];
dehole=[[decornerr,decornerr],[decornerr,dey-decornerr],
        [dex-decornerr-10,decornerr],[dex-decornerr-10,dey-decornerr],
        [dex-51.5,dey-20.5],[dex-51.5,dey-(55+20.5)]
        
        ];

//deholero = 6.35/2;
deholero = 4.5;
deholeri1 = 3.8;
deholeri2 = 3.8/2;

pcsh = 3.8/2;
deholeh=6;
tabx=10;
taby=20;

demount = [[-mountoff,dey/2],[dex+mountoff,dey/2]];

doit(holes,deouter,dehole,decornerr,debaseh,deholero,deholeri1,deholeri2,deholeh,csh = pcsh);
doit(holes,concat(deouter,demount),demount,mountw,debaseh,mountro,mountri,mountri,mounth);
        
      
};

module usbhub(holes=0) {
w=24.6+0.5;
h=49.8+0.5;
m=1.5;
im=1.5;
lip=1.5;
height=8;
//imountr=2.3/2;
imountr0=4.4/2;
imountr1=4.1/2;
//omountr= 5/2;
omountr=6.5/2;
mountoff=4;
holeoff=3;
bmountoff=imountr1-(2.5/2);
difference() {
union() {
        
        cube([w+2*m,h+2*m,height]);
        hull() {
        translate([0,h/4+m,0])
           cube([w+2*m,h/2,height]);
           
        translate([-holeoff,m+h/2,0])
        cylinder(r=mountro,h=height);
        translate([holeoff+w+2*m,m+h/2,0])
        cylinder(r=mountro,h=height);
            };
    translate([m+w/2,m+imountr1+h-bmountoff,0])
        cylinder(r=omountr,h=height);
    translate([m+mountoff,m-imountr1+bmountoff,0])
        cylinder(r=omountr,h=height);
};
translate([m+0.01,m+0.01,height-lip])
    cube([w-0.02,h-0.02,height]);
translate([m+im,m+im,-0.01])
    cube([w-2*im,h-2*im,height+0.1]);

translate([m+w/2,m+imountr1+h-bmountoff,-0.01])
    cylinder(r1=imountr0,r2=imountr1,h=1+0.1);
translate([m+mountoff,m-imountr1+bmountoff,-0.01])
    cylinder(r1=imountr0,r2=imountr1,h=1+0.1);

translate([m+w/2,m+imountr1+h-bmountoff,-0.01])
    cylinder(r1=imountr1,r2=imountr1,h=height+0.1);
translate([m+mountoff,m-imountr1+bmountoff,-0.01])
    cylinder(r1=imountr1,r2=imountr1,h=height+0.1);

translate([-holeoff,m+h/2,-0.01])
        cylinder(r=mountri,h=height+1);
translate([holeoff+w+2*m,m+h/2,-0.01])
        cylinder(r=mountri,h=height+1);
translate([-holeoff,m+h/2,mounth])
        cylinder(r=mountro+1,h=height);
translate([holeoff+w+2*m,m+h/2,mounth])
        cylinder(r=mountro+1,h=height);
    
};


};

module powerdist(holes=0) {
w=30+0.5;
h=70+0.5;
m=1.5;
im=1.5;
lip=1.5;
height=8;
//imountr=2.3/2;
imountr0=4.4/2;
imountr1=4.1/2;
//omountr= 5/2;
omountr=6.5/2;
mountoff=4;
holeoff=3;
bmountoff=imountr1-(2.5/2);
edgemount(w,h,m,im,lip,height,imountr0,imountr1,omountr,mountoff,holeoff,bmountoff);
};

module ecpcb(holes=0) {
w=44.75+0.5;
h=48.5+0.5;
m=1.5;
im=1.5;
lip=1.5;
height=8;
//imountr=2.3/2;
imountr0=4.4/2;
imountr1=4.1/2;
//omountr= 5/2;
omountr=6.5/2;
mountoff=4;
holeoff=3;
bmountoff=imountr1-(2.5/2);
edgemount(w,h,m,im,lip,height,imountr0,imountr1,omountr,mountoff,holeoff,bmountoff);
};

module edgemount(w,h,m,im,lip,height,imountr0,imountr1,omountr,mountoff,holeoff,bmountoff) {

difference() {
union() {
        
        cube([w+2*m,h+2*m,height]);
        hull() {
        translate([0,h/4+m,0])
           cube([w+2*m,h/2,height]);
           
        translate([-holeoff,m+h/2,0])
        cylinder(r=mountro,h=height);
        translate([holeoff+w+2*m,m+h/2,0])
        cylinder(r=mountro,h=height);
            };
    translate([m+w/2,m+imountr1+h-bmountoff,0])
        cylinder(r=omountr,h=height);
    translate([m+w/2,m-imountr1+bmountoff,0])
        cylinder(r=omountr,h=height);
};
translate([m+0.01,m+0.01,height-lip])
    cube([w-0.02,h-0.02,height]);
translate([m+im,m+im,-0.01])
    cube([w-2*im,h-2*im,height+0.1]);

translate([m+w/2,m+imountr1+h-bmountoff,-0.01])
    cylinder(r1=imountr0,r2=imountr1,h=1+0.1);
translate([m+w/2,m-imountr1+bmountoff,-0.01])
    cylinder(r1=imountr0,r2=imountr1,h=1+0.1);

translate([m+w/2,m+imountr1+h-bmountoff,-0.01])
    cylinder(r1=imountr1,r2=imountr1,h=height+0.1);
translate([m+w/2,m-imountr1+bmountoff,-0.01])
    cylinder(r1=imountr1,r2=imountr1,h=height+0.1);

translate([-holeoff,m+h/2,-0.01])
        cylinder(r=mountri,h=height+1);
translate([holeoff+w+2*m,m+h/2,-0.01])
        cylinder(r=mountri,h=height+1);
translate([-holeoff,m+h/2,mounth])
        cylinder(r=mountro+1,h=height);
translate([holeoff+w+2*m,m+h/2,mounth])
        cylinder(r=mountro+1,h=height);
    
};


};


module kvm(holes=0) {
kvmx=58;
kvmy=100;
kvmcornerr=3;
kvmbaseh=baseh;
kvmouter = [[0-kvmcornerr,0-kvmcornerr],[kvmx+kvmcornerr,0-kvmcornerr],
    [0-kvmcornerr,kvmy+kvmcornerr],[kvmx+kvmcornerr,kvmy+kvmcornerr]];
khr = 3.32/2;
kvmhole=[[15.47 + khr,6.05+khr],[37.90+khr,57.8+khr],[15.5+khr,(90.7+khr)]];
// 6.38 + (3.32/2)
// 15.47 + (3.32/2)
// 37.90 ...
// 57.75
// 15.5
// 90.5
// 
//6.01
//90.77
//57.99
//6.06
//90.67
kvmholero = 3;
// piholeri1 = 2.7/2;
// piholeri2 = 2.7/2;
kvmholeri1 = mountr0;
kvmholeri2 = mountr1;
// 3.4/2,3.8/2
kvmholeh=6;


kvmmount = [[-mountoff-3,kvmy/2],[3+kvmx+mountoff,kvmy/2]];

doit(holes,concat(kvmouter,kvmhole),kvmhole,kvmcornerr,kvmbaseh,kvmholero,kvmholeri1,kvmholeri2,kvmholeh);
doit(holes,concat(kvmouter,kvmmount),kvmmount,mountw,kvmbaseh,mountro, mountri,mountri,mounth);
if (holes == 0 ) {
//translate([-5,-1,0]) cube([kvmx+5,kvmy+2,kvmbaseh]);
//translate([-10,25,0]) cube([kvmx+20,kvmy-50,kvmbaseh]);
holeat(0,0,2,2,kvmholeh);
holeat(kvmx,0,2,2,kvmholeh);
holeat(kvmx,kvmy,2,2,kvmholeh);
holeat(0,kvmy,2,2,kvmholeh);
}

      
};


module relay(v=1,holes=0) {
relayxv=[38,47.352];
relayx=relayxv[v-1];
relayyv = [50,72];
relayy=relayyv[v-1];
relaycornerrxv=[2.5,3];
relaycornerryv=[3,3];
relaycornerrx=relaycornerrxv[v-1];
relaycornerry=relaycornerryv[v-1];
relaycornerr=relaycornerry;
relaybaseh=baseh;

// W=36 30 :: 33 38
// 41 47 :: 50 44


relayouter = [[0-relaycornerr,0-relaycornerr],[relayx+relaycornerr,0-relaycornerr],
    [0-relaycornerr,relayy+relaycornerr],[relayx+relaycornerr,relayy+relaycornerr]];
relayhole=[[relaycornerrx,relaycornerry],
            [relaycornerrx,relayy-relaycornerry],
            [relayx-relaycornerrx,relayy-relaycornerry],
            [relayx-relaycornerrx,relaycornerry]];
    relayholero = [5,3];
//relayholeri1 = 3.8/2;
//relayholeri2 = 3.4/2;
relayholeri1=mountr0;
relayholeri2=mountr1;

relayholeh=6;
relaymount = [[-mountoff-relaycornerrx,relayy/2],[relaycornerrx+relayx+mountoff,relayy/2]];



doit(holes,concat(relayouter,relayhole),relayhole,relaycornerr,relaybaseh,relayholero,relayholeri1,relayholeri2,relayholeh);
doit(holes,concat(relayouter,relaymount),relaymount,mountw,relaybaseh,mountro, mountri,mountri,mounth);
      
};

module switch(holes=0) {
switchx=70;
switchy=55;
switchcornerr=3;
switchbaseh=baseh;

switchcx=(70-64)/2;
switchcy=(55-48.725)/2;

// 45.80 51.65  55
// 70 67.2 60.8
switchouter = [[0-switchcornerr,0-switchcornerr],[switchx+switchcornerr,0-switchcornerr],
    [0-switchcornerr,switchy+switchcornerr],[switchx+switchcornerr,switchy+switchcornerr]];
switchhole=[[switchcx,switchcy],[switchcx,switchy-switchcy],
            [switchx-switchcx,switchy-switchcy],
            [switchx-switchcx,switchcy]];
    switchholero = [5,3];
switchholeri1 = mountr0;
switchholeri2 = 3.7/2;

switchholeh=6+2;
switchmount = [[-mountoff-3,switchy/2],[3+switchx+mountoff,switchy/2]];



doit(holes,concat(switchouter,switchhole),switchhole,switchcornerr,switchbaseh,switchholero,switchholeri1,switchholeri2,switchholeh);
doit(holes,concat(switchouter,switchmount),switchmount,mountw,switchbaseh,mountro, mountri,mountri,mounth);
      
};



difference () {

//cube([x,y,1]);
union() {
if (!is_undef(pixy)) translate(pixy) pi(holes=0);
if (!is_undef(rockxy)) translate(rockxy) rock(holes=0);
if (!is_undef(dexy)) translate(dexy) de10n(holes=0);
//translate(picoxy) pico(holes=0);
//translate(hubxy) usbhub(holes=0);
if (!is_undef(powerdistxy)) translate(powerdistxy) powerdist(holes=0);
if (!is_undef(ecpcbxy)) translate(ecpcbxy) ecpcb(holes=0);
//translate(kvmxy) kvm(holes=0);
if (!is_undef(relayxy1)) translate(relayxy1) relay(v=1,holes=0);
if (!is_undef(relayxy2)) translate(relayxy2) relay(v=2,holes=0);
//translate(switchxy) switch(holes=0);
//translate(powerxy) power(holes=0);
//mixmesh(holes,holes,hro,hh);
//translate([xminus-x,-yminus,0]) cube([x,37,hh]);
//translate([xminus-xca-wca,2-yminus,0]) cube([wca,2,10]);
};
if (!is_undef(pixy)) translate(pixy) pi(holes=1);
if (!is_undef(rockxy)) translate(rockxy) rock(holes=1);
if (!is_undef(dexy)) translate(dexy) de10n(holes=1);
// translate(picoxy) pico(holes=1);
//    translate(powerxy) power(holes=1);
//      for (j=holes) {
//        holeat(j[0],j[1],hri,hri,hh,e);
//    translate(kvmxy) kvm(holes=1);
if (!is_undef(relayxy1)) translate(relayxy1) relay(v=1,holes=1);
if (!is_undef(relayxy2)) translate(relayxy2) relay(v=2,holes=1);
//translate(switchxy) switch(holes=1);
//};  
};
