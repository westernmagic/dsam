function EuclidDist=findEuclideanDistance(outputLevels,maskerLevels,gaps)
%find Euclidean distance

figure (1),clf,   plotColors=['rgbkycm'];

[nGaps nFreqs]=size(maskerLevels); 
EuclidDist=0; N=0;
for j=1:nFreqs
   for i=1:nGaps,
      if  ~isnan(outputLevels(i,j))
         EuclidDist=EuclidDist+(outputLevels(i,j)-maskerLevels(i,j))^2;
         N=N+1;
      end
   end
   plot(outputLevels(:,j),gaps,plotColors(j)); hold on
   axis([20 100 10 100])
end

for i=1:6,plot(maskerLevels(:,i),gaps,[plotColors(i) 'o']),hold on,end
   ylabel('gap (msec)');
   xlabel ('maskerLevel');

    figure (2),clf
   for i=1:nFreqs,plot(gaps,maskerLevels(:,i),[plotColors(i) 'o']),hold on,end
   xlabel('gap (msec)');
   ylabel ('maskerLevel');
   pause(.1)
   
  
   for i=1:nFreqs,plot(gaps,outputLevels(:,i),plotColors(i)),hold on,end
   xlabel('gap (msec)');
   ylabel ('maskerLevel');


pause(.1)
EuclidDist=sqrt(EuclidDist/N);

