for ARGUMENT in "$@"
do
    KEY=$(echo $ARGUMENT | cut -f1 -d=)
    VALUE=$(echo $ARGUMENT | cut -f2 -d=)   
    case "$KEY" in
            espruino)  ESPRUINO=${VALUE} ;;
            espidf)    ESPIDF=${VALUE} ;;     
            *)   
    esac    
done
