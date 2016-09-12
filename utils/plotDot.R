args <- commandArgs(trailingOnly = TRUE)

require(ggplot2)

dat<-read.table(args[1])

start<-dat[dat$V2 == 16,]$V6
end<-dat[dat$V2 == 16,]$V7

dat[dat$V2 == 16,]$V6 <- end
dat[dat$V2 == 16,]$V7 <- start

theplot<-ggplot(dat, aes(y=V6/1e3, yend=V7/1e3, x=V9/1e3, xend=V10/1e3, colour=factor(V1)))+geom_segment(size=1)+labs(x="Target Kbps", y="Query Kbps", title=paste(names(table(dat$V3)), "vs"  , names(table(dat$V1))))+theme_bw(16)

ggsave(paste(names(table(dat$V3))[1], "vs"  , names(table(dat$V1))[1], ".pdf", sep="-"), theplot)