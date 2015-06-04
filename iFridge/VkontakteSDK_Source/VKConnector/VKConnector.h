//
//  VKConnector.h
//
//  Created by Andrew Shmig on 18.12.12.
//
//
// Copyright (c) 2013 Andrew Shmig
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#import <Foundation/Foundation.h>
#import "VKMethods.h"

@class VKAccessToken;
@class VKConnector;
@class KGModal;


/** Протокол объявляет методы отслеживания изменения статуса токена доступа
 хранимого классом VKConnector.
 */
@protocol VKConnectorProtocol <NSObject>

@optional
/**
 @name Modal view
 */
/** Метод вызывается до того, как произойдет отображение модального окна авторизации
 
 @param connector объект класса VKConnector отправляющий сообщение
 @param view модальное окно
 */
- (void)VKConnector:(VKConnector *)connector
  willShowModalView:(KGModal *)view;

/** Метод вызывается до того, как произойдет скрытие модального окна авторизации
 
 @param connector объект класса VKConnector отправляющий сообщение
 @param view модальное окно
 */
- (void)VKConnector:(VKConnector *)connector
  willHideModalView:(KGModal *)view;

/**
 @name Access token
 */
/** Метод, вызов которого сигнализирует о том, что токен стал недействительным,
 срок его действия истёк.
 
 @param connector объект класса VKConnector отправляющий сообщение.
 @param accessToken токен доступа, срок действия которого истёк.
 */
- (void)VKConnector:(VKConnector *)connector
accessTokenInvalidated:(VKAccessToken *)accessToken;

/** Метод, вызов которого сигнализирует о том, что токен доступа успешно обновлён.
 
 @param connector объект класса VKConnector отправляющий сообщение.
 @param accessToken новый токен доступа, который был получен.
 */
- (void)VKConnector:(VKConnector *)connector
 accessTokenRenewalSucceeded:(VKAccessToken *)accessToken;

/** Метод, вызов которого сигнализирует о том, что обновление токена доступа не
 удалось.
 Причиной ошибки может быть прерывание связи, либо пользователь отказался авторизовывать
 приложение.
 
 @param connector объект класса VKConnector отправляющего сообщение.
 @param accessToken токен доступа (равен nil)
 */
- (void)VKConnector:(VKConnector *)connector
accessTokenRenewalFailed:(VKAccessToken *)accessToken;

/**
 @name Connection & Parsing
 */
/** Метод, вызов которого сигнализирует о том, что произошла ошибка соединения при попытке осуществить запрос
 
 @param connector объект класса VKConnector отправляющего сообщение
 @param error объект ошибки содержащий описание причины возникновения ошибки
 */
- (void)VKConnector:(VKConnector *)connector
connectionErrorOccured:(NSError *)error;

/** Метод, вызов которого сигнализирует о том, что произошла ошибка при парсинге JSON ответа сервера
 
 @param connector объект класса VKConnector отправляющего сообщение
 @param error объект ошибки содержащий описание причины возникновения ошибки
 */
- (void)VKConnector:(VKConnector *)connector
parsingErrorOccured:(NSError *)error;

@end






/** Класс предназначен для получения приложением доступа к пользовательской учетной
 записи и осуществления запросов к API сервиса методами GET/POST.

    - (void)applicationDidFinishLaunching:(UIApplication *)application
    {
        [[VKConnector sharedInstance] startWithAppID:@"YOUR_APP_ID"
                                          permissons:@[@"friends",@"wall"]];
    }

 */
@interface VKConnector : NSObject <UIWebViewDelegate>

/**
@name Свойства
*/
/** Делегат VKConnector
 */
@property (nonatomic, weak) id<VKConnectorProtocol> delegate;

/** Идентификатор приложения Вконтакте
 */
@property (nonatomic, strong, readonly) NSString *appID;

/** Список разрешений
 */
@property (nonatomic, strong, readonly) NSArray *permissions;

/**
@name Методы класса
*/
/** Метод класса для получения экземпляра сиглтона.
* Если объект отсутствует, то он будет создан. Не может быть равен nil или NULL.
*/
+ (id) sharedInstance;

/**
@name Авторизация пользователем приложения
*/
/** Инициализирует запуск коннектора с заданными параметрами

 @param appID Идентификатор приложения полученный при регистрации.
 @param permissions Массив доступов (разрешений), которые необходимо получить приложению.
 */
- (void)startWithAppID:(NSString *)appID
            permissons:(NSArray *)permissions;

/** Производит выход текущего пользователя из учетной записи - удаляет куки.
*/
- (void)logout;

/**
@name Осуществление запросов
*/
/** Основной метод осуществления GET запросов.
 
 @param methodName Наименование метода к которому необходимо осуществить запрос.
 Со списком методов можно ознакомиться в документации по ссылке: https://vk.com/dev/methods
 либо в заголовочном файле VKMethods.h
 
 _Возможные значения:_ kVKUsersGet, @"users.get" и тд
 
 @param options Словарь ключей-значений требуемых для корректного осуществления запроса и получения информации.
 С каждым списком параметров можно ознакомиться в документации соответствующего метода.
 
 @param error В случае возникновения ошибки она будет передана в данной переменной. Ошибки могут быть двух типов:
 ошибка запроса (прервалось соединение и тд) и ошибки парсинга ответа сервера.
 Возвращаемые ошибки сервером в запросе не подвергаются обработке.
 */
- (id)performVKMethod:(NSString *)methodName
              options:(NSDictionary *)options
                error:(NSError **)error;

/** Осуществляет переданый запрос

@param request запрос, который необходимо осуществить
@return ответ на запрос в виде Foundation объекта
*/
- (id)performVKRequest:(NSMutableURLRequest *)request;

/** Загружает файл на указанный адрес

@param file байтовое представление газружаемого файла
@param fileName наименование файла
@param url URL на который необходимо отправить данные
@param contentType тип загружаемого содержимого (application/octet-stream, image/jpeg и тд)
@param fieldName наименования поля загрузки (file, picture, pics и тд)

@return ответ на запрос в виде Foundation объекта
*/
- (id)uploadFile:(NSData *)file
            name:(NSString *)fileName
             URL:(NSURL *)url
     contentType:(NSString *)contentType
       fieldName:(NSString *)fieldName;

@end
